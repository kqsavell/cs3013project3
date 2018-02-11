// Kyle Savell & Antony Qin
// Project 3: Bathroom

#include "bathroom.h"

// Actual bathroom structure
struct monitor *bathroom;

// Counts total number of threads that are created
int total_threads = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t plock = PTHREAD_MUTEX_INITIALIZER; // Lock for printing output
pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;

/* -- Functions -- */

// Gets a random normal distribution value based on mean and standard deviation
int norm_dist(int mean, float std)
{
    // Get the random values
    float a = drand48();
    float b = drand48();

    // Calculate using normal distribution
    float x = (sqrt(-2 * log(a)) * cos(2 * M_PI * b));
    return (int)((x * std) + mean);
}

// Initialize global variables
void Initialize()
{
    // Create bathoom
    bathroom = malloc(sizeof(struct monitor));
    bathroom->status = vacant;
    bathroom->numUsages = 0;
    bathroom->totalTime = 0;
    bathroom->currPpl = 0;
}

void Finalize()
{
    // Prints statistics for bathroom
    printf("\nStatistics of Bathroom:\n");
    printf("Total threads: %d\n", total_threads);
    printf("Number of times bathroom used: %d\n", bathroom->numUsages);
    printf("Total time bathroom was not empty (microsseconds): %.3f\n", bathroom->totalTime);
}

// Function that runs inside the thread that tries to enter/leave bathroom
void* Individual(void *input)
{
    // Copy data from input struct to local variables
    struct args *my_args = (struct args *)input;
    enum gender per_gender = my_args->person;
    int mean_arrival = my_args->arrival;
    int mean_stay = my_args->stay;
    int num_loops = my_args->loops;
    free(input);

    // Define additional local variables
    struct timeval wait_start;
    struct timeval wait_end;
    int wait_counter = 0;
    float wait_min = 0;
    float wait_max = 0;
    float wait_ave = 0;

    // Wait to use bathroom
    int rand_stay = norm_dist(mean_arrival, (float)mean_stay/2);
    if(rand_stay <= 0)
        rand_stay = 1;
    usleep(rand_stay);

    // Attempt for thread to enter the bathroom
    void Enter(enum gender g)
    {
        pthread_mutex_lock(&lock);

        char* gender;
            if((int)g == 0)
                gender = "A guy";
            else
                gender = "A gal";

        while(bathroom->status != vacant && bathroom->status != g+1) // Not empty and filled with opposite gender
        {
            // Start current wait timer
            gettimeofday(&wait_start, NULL);
            wait_counter += 1;

            pthread_mutex_lock(&plock);
            printf("Yes, %ld (%s) am waiting because the bathroom is occupied with the opposite gender.\n", pthread_self(), gender);
            pthread_mutex_unlock(&plock);

            // Wait for bathroom to be vacant
            pthread_cond_wait(&g_cond, &lock);

            // Queue statistics
            gettimeofday(&wait_end, NULL);
            float total_wait = ((wait_end.tv_sec - wait_start.tv_sec)*1000 + ((float)wait_end.tv_usec - (float)wait_start.tv_usec)/1000);
            if ((wait_min == 0) || (total_wait < wait_min)) // Min waiting time for thread
            {
                wait_min = total_wait;
            }
            if ((wait_max == 0) || (total_wait > wait_max)) // Max wiating time for thread
            {
                wait_max = total_wait;
            }
            wait_ave += total_wait; // Average waiting time (not divded yet)
        }

        if ((bathroom->status == vacant) || (bathroom->status == g+1))
        {
            // If vacant, start current stay timer
            if (bathroom->status == vacant)
            {
                gettimeofday(&bathroom->curStart, NULL);
            }

            // Increment bathroom counters and change status
            bathroom->currPpl++;
            bathroom->status = g+1;
            bathroom->numUsages++;

            pthread_mutex_lock(&plock);
            printf("%ld (%s) has entered the bathroom\n", pthread_self(), gender);
            pthread_mutex_unlock(&plock);
        }

        pthread_mutex_unlock(&lock);

        // Stay in bathroom for normally distrubted time
        rand_stay = norm_dist(mean_stay, (float)mean_stay/2);
        if(rand_stay <= 0)
            rand_stay = 1;
        usleep(rand_stay);
    }

    // Thread leaves the bathroom
    void Leave()
    {
        pthread_mutex_lock(&lock);

        // Decrement people in bathroom
        bathroom->currPpl--;

        // If bathroom is empty, get stay time and send broadcast
        if(bathroom->currPpl == 0)
        {
            // End current stay timer
            gettimeofday(&bathroom->curEnd, NULL);
            float total_stay = ((bathroom->curEnd.tv_sec - bathroom->curStart.tv_sec)*1000 + ((float)bathroom->curEnd.tv_usec - (float)bathroom->curStart.tv_usec)/1000);

            // Change status of bathroom
            bathroom->totalTime += total_stay;
            bathroom->status = vacant;

            // Send signal to all waiting threads
            pthread_cond_broadcast (&g_cond);

            pthread_mutex_lock(&plock);
            printf("BATHROOM IS EMPTY\n\n");
            pthread_mutex_unlock(&plock);
        }

        pthread_mutex_unlock(&lock);
    }

    //printf("Loop Count: %d\n", num_loops);
    if (num_loops > 0)
    {
        for (int i = 0; i < num_loops; i++)
        {
            // Try to Enter
            Enter(per_gender);
            // Leave
            Leave();
        }

        // Get thread's gender
        char* gender;
            if((int)per_gender == 0)
                gender = "A Guy";
            else
                gender = "A Gal";

        // Print thread data
        if (wait_counter != 0) {
            wait_ave = wait_ave/wait_counter;
        }
        pthread_mutex_lock(&plock);
        printf("\nThread %ld has finished:\n", pthread_self());
        printf("Gender: %s\n", gender);
        printf("Number of loops: %d\n", num_loops);
        printf("Max wait %.3f: Min wait %.3f: Average wait: %.3f\n\n", wait_max, wait_min, wait_ave);
        pthread_mutex_unlock(&plock);

    }

    return 0;
}
