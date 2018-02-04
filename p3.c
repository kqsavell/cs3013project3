// Kyle Savell & Antony Qin
// Project 3: Bathroom

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

int norm_dist(void) {
    float a = drand48();
    float b = drand48();
    return (sqrt(-2 * log(a)) * cos(2 * M_PI * b));
}

enum gender{male, female};
struct args {
    enum gender person;
    int arrival;
    int stay;
    int loops;
};

// Function that runs inside the thread that tries to enter/leave bathroom
void* Individual(void *input) {
    struct args *my_args;
    my_args = (struct args *)input;
    enum gender per_gender = my_args->person;
    int mean_arrival = my_args->arrival;
    int mean_stay = my_args->stay;
    int num_loops = my_args->loops;
    printf("test %d\n", *(int *)per_gender);

    return 0;
}

// Main function
int main(int argc, char *argv[]) {
    srand(time(NULL));
    srand48(time(NULL));
    printf("Main Function\n");
    pthread_t thread;
    int num_users = 100;
    int mean_arrival = 10;
    int mean_stay = 10;
    int mean_loop = 10;

    // Read command line
    for (int i = 1; i < argc; i++) {
        if (i == 1) {
            num_users = atoi(argv[i]);
        }
        else if (i == 2) {
            mean_arrival = atoi(argv[i]);
        }
        else if (i == 3) {
            mean_stay = atoi(argv[i]);
        }
        else if (i == 4) {
            mean_loop = atoi(argv[i]);
        }
        else {
            printf("ERROR: Too many input arguments!\n");
            return 1;
        }
    }

    //Enter thread-making loop
    for (int i = 0; i < num_users; i++) {
        // Generate gender
        enum gender per_gender;
        int gender_int = rand() % 2; // Either 0 or 1
        if (gender_int == 0) {
            per_gender = male;
        }
        else {
            per_gender = female;
        }

        // Generate number of loops
        int loop_count = mean_loop*norm_dist();
        printf("%d\n", loop_count);

        // Fill struct
        struct args *new_args;
        new_args = malloc(sizeof(struct args));
        new_args->person = per_gender;
        new_args->arrival = mean_arrival;
        new_args->stay = mean_stay;
        new_args->loops = mean_loop;

        pthread_create(&thread, NULL, Individual, &new_args);
        pthread_join(thread, NULL);
    }
    return 0;
}
