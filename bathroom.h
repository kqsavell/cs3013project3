// Kyle Savell & Antony Qin
// Project 3: Bathroom

#ifndef FILE_BATHROOM_SEEN
#define FILE_BATHROOM_SEEN

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

/* -- Definitions -- */

enum gender {male, female};
enum filled_with {vacant, males, females};
int total_threads;

// Locks
pthread_mutex_t lock;
pthread_mutex_t plock; // Lock for printing output
pthread_cond_t g_cond;

// Struct for input arguments for threads
struct args
{
    enum gender person;
    int arrival;
    int stay;
    int loops;
};

// Struct for bathroom
struct monitor
{
    int occupants[100];
    enum filled_with status;

    int numUsages;
    float totalTime;
    float currPpl;
    struct timeval curStart;
    struct timeval curEnd;
};

// Functions
int norm_dist(int mean, float std);
void* Individual(void *input);
void Enter(enum gender g); //Enter bathroom, set state if needed
void Leave(void); //Leave bathroom, set state if needed
void Initialize();
void Finalize();

#endif

