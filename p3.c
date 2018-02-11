// Kyle Savell & Antony Qin
//Project 3: Bathroom Sim

#include "bathroom.h"

// Main function
int main(int argc, char *argv[])
{
    // Random seed values
    srand(time(NULL));
    srand48(time(NULL));

    // Default values for user input
    int num_users = 2;
    int mean_loop = 3;
    int mean_arrival = 10;
    int mean_stay = 10;

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
            mean_loop = atoi(argv[i]);
        }
        else if (i == 3)
        {
            mean_arrival = atoi(argv[i]);
        }
        else if (i == 4)
        {
            mean_stay = atoi(argv[i]);
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

        // Create thread
        if (pthread_create(&threads[i], NULL, Individual, (void*)new_args) == 0)
        {
            total_threads++;
        }
    }

    // Wait for all threads to finish
    for(int i = 0; i < num_users; i++)
        pthread_join(threads[i], NULL);

    // Print final bathroom statistics
    Finalize();

    return 0;
}
