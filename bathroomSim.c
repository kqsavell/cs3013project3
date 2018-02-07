// Kyle Savell & Antony Qin
// Project 3: Bathroom

#include "bathroom.h"

// Actual bathroom structure
struct monitor *bathroom;

int total_threads = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t plock = PTHREAD_MUTEX_INITIALIZER; // Lock for printing output
pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;

/* -- Functions -- */

int norm_dist(int mean, float std)
{
    float a = drand48();
    float b = drand48();
    float x = (sqrt(-2 * log(a)) * cos(2 * M_PI * b));
    //printf("%f\n", x);
    //int y = (sqrt(-2 * log(a)) * sin(2 * M_PI * b));
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
    bathroom->avgQueue = 0;
    bathroom->avgPpl = 0;
}

void Finalize()
{
    printf("\nStatistics of Bathroom:\n");
    printf("Total threads: %d\n", total_threads);
    printf("Number of times bathroom used: %d\n", bathroom->numUsages);
    printf("Total time bathroom was not empty (ms): %.3f\n", bathroom->totalTime);
}

// Function that runs inside the thread that tries to enter/leave bathroom
void* Individual(void *input)
{
    struct args *my_args = (struct args *)input;

    enum gender per_gender = my_args->person;
    int mean_arrival = my_args->arrival;
    int mean_stay = my_args->stay;
    int num_loops = my_args->loops;
    free(input);

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
    sleep(rand_stay * 0.01);
    //printf("I waited for %d units of time\n", rand_stay);

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
            gettimeofday(&wait_start, NULL);
            wait_counter += 1;
            printf("Yes, I (%s) am waiting because the bathroom is occupied with the opposite gender.\n", gender);
            pthread_cond_wait(&g_cond, &lock);
            gettimeofday(&wait_end, NULL);
            float total_wait = ((wait_end.tv_sec - wait_start.tv_sec)*1000 + ((float)wait_end.tv_usec - (float)wait_start.tv_usec)/1000);
            if ((wait_min == 0) || (total_wait < wait_min))
            {
                wait_min = total_wait;
            }
            if ((wait_max == 0) || (total_wait > wait_max))
            {
                wait_max = total_wait;
            }
            wait_ave += total_wait;

        }

        if ((bathroom->status == vacant) || (bathroom->status == g+1))
        {
            if (bathroom->status == vacant)
            {
                gettimeofday(&bathroom->curStart, NULL);
            }
            bathroom->avgPpl++;
            bathroom->status = g+1;
            bathroom->numUsages++;

            printf("%s has entered the bathroom\n", gender);
        }

        pthread_mutex_unlock(&lock);

        rand_stay = norm_dist(mean_stay, (float)mean_stay/2);
        if(rand_stay <= 0)
            rand_stay = 1;
        sleep(rand_stay * 0.01);
        //printf("I stayed for %d units of time\n", rand_stay);
    }

    // Thread leaves the bathroom
    void Leave()
    {
        pthread_mutex_lock(&lock);

        //printf("Occupant is leaving the bathroom\n\n");

        bathroom->avgPpl--;
        if(bathroom->avgPpl == 0)
        {
            gettimeofday(&bathroom->curEnd, NULL);
            //printf("Start time s: %ld, ms: %ld\n", bathroom->curStart.tv_sec, bathroom->curStart.tv_usec);
            //printf("End time s: %ld, ms : %ld\n", bathroom->curEnd.tv_sec, bathroom->curEnd.tv_usec);
            float total_stay = ((bathroom->curEnd.tv_sec - bathroom->curStart.tv_sec)*1000 + ((float)bathroom->curEnd.tv_usec - (float)bathroom->curStart.tv_usec)/1000);
            bathroom->totalTime += total_stay;
            bathroom->status = vacant;
            pthread_cond_signal (&g_cond);
             printf("BATHROOM IS EMPTY\n\n");
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

// Main function
int main(int argc, char *argv[])
{
    srand(time(NULL));
    srand48(time(NULL));
    printf("Main Function\n");

    int num_users = 100;
    int mean_arrival = 10;
    int mean_stay = 10;
    int mean_loop = 3;

    Initialize(); // Initialize bathroom

    // Read command line
    for (int i = 1; i < argc; i++)
    {
        if (i == 1)
        {
            num_users = atoi(argv[i]);
        }
        else if (i == 2)
        {
            mean_arrival = atoi(argv[i]);
        }
        else if (i == 3)
        {
            mean_stay = atoi(argv[i]);
        }
        else if (i == 4)
        {
            mean_loop = atoi(argv[i]);
        }
        else
        {
            printf("ERROR: Too many input arguments!\n");
            return 1;
        }
    }

    struct args *new_args;
    //Enter thread-making loop
    pthread_t threads[num_users];
    for (int i = 0; i < num_users; i++)
    {

        // Generate gender
        enum gender per_gender;
        int gender_int = rand() % 2; // Either 0 or 1
        if (gender_int == 0)
        {
            per_gender = male;
        }
        else
        {
            per_gender = female;
        }

        // Generate number of loops
        int loop_count = norm_dist(mean_loop, (float)mean_loop/2);
        if(loop_count <= 0)
            loop_count = 1;

        // Fill struct

        new_args = malloc(sizeof(struct args));
        new_args->person = per_gender;
        new_args->arrival = mean_arrival;
        new_args->stay = mean_stay;
        new_args->loops = loop_count;

        pthread_mutex_lock(&plock);
        printf("mean arrival is: %d\n", new_args->arrival);
        printf("mean stay is: %d\n", new_args->stay );
        printf("mean loop count is: %d\n", new_args->loops);
        pthread_mutex_unlock(&plock);

        if (pthread_create(&threads[i], NULL, Individual, (void*)new_args) == 0)
        {
            total_threads++;
        }
    }
    for(int i = 0; i < num_users; i++)
        pthread_join(threads[i], NULL);

    Finalize();

    return 0;
}
