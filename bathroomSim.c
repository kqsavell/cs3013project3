// Kyle Savell & Antony Qin
// Project 3: Bathroom

#include "bathroom.h"

// Actual bathroom structure
struct monitor *bathroom;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;

/* -- Functions -- */

int norm_dist(int mean, float std)
{
    float a = drand48();
    float b = drand48();
    float x = (sqrt(-2 * log(a)) * cos(2 * M_PI * b));
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
    printf("Statistics of Bathroom:\n");
    printf("Number of times used: %d\n", bathroom->numUsages);
    printf("Total time used: %d\n", bathroom->totalTime);
    printf("Average time spent waiting: %.2f\n", bathroom->avgQueue);
    printf("Average people in at a time: %.2f\n", bathroom->avgPpl);
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

    // Attempt for thread to enter the bathroom
    void Enter(enum gender g)
    {
        pthread_mutex_lock(&lock);

        while(bathroom->status != vacant && bathroom->status != g+1) // Not empty and filled with opposite gender
        {
            printf("Yes, I am waiting because the bathroom is occupied with the opposite gender.");
            pthread_cond_wait(&g_cond, &lock);
        }

        if ((bathroom->status == vacant) || (bathroom->status == g+1))
        {
            bathroom->avgPpl++;
            bathroom->status = g+1;

            char* gender;
            if((int)g == 0)
                gender = "A guy";
            else
                gender = "A gal";
            printf("%s has entered the bathroom\n", gender);
        }

        pthread_mutex_unlock(&lock);
        int rand_stay = norm_dist(mean_stay, (float)mean_stay/2);
        if(rand_stay <= 0)
            rand_stay = 1;
        sleep(rand_stay * 0.01);
        printf("I stayed for %d units of time\n", rand_stay);
    }

    // Thread leaves the bathroom
    void Leave()
    {
        pthread_mutex_lock(&lock);

        printf("Occupant is leaving the bathroom\n\n");

        bathroom->avgPpl--;
        if(bathroom->avgPpl == 0)
            bathroom->status = vacant;

        pthread_mutex_unlock(&lock);
    }

    printf("Loop Count: %d\n", num_loops);
    if (num_loops > 0)
    {
        for (int i = 0; i < num_loops; i++)
        {
            // Try to Enter
            Enter(per_gender);
            // Leave
            Leave();
        }
    }

    return 0;
}

// Main function
int main(int argc, char *argv[])
{
    srand(time(NULL));
    srand48(time(NULL));
    printf("Main Function\n");
    pthread_t thread;
    int num_users = 10;
    int mean_arrival = 10;
    int mean_stay = 100;
    int mean_loop = 5;

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
        printf("mean arrival is: %d\n", new_args->arrival);
        printf("mean stay is: %d\n", new_args->stay );
        printf("mean loop count is: %d\n", new_args->loops);

        pthread_create(&thread, NULL, Individual, (void*)new_args);
        pthread_join(thread, NULL);
    }

    Finalize();

    return 0;
}
