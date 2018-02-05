// Kyle Savell & Antony Qin
// Project 3: Bathroom

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

enum gender {male, female};
void Enter(enum gender g); //Enter bathroom, set state if needed
void Leave(void); //Leave bathroom, set state if needed
void Initialize();
void Finalize();

int numUsages; // Number of times bathroom used
int totalTime; // Total time bathroom used

float avgQueue; // Average wait time to use bathroom
int totalWaits; // Total number of waits

float avgPpl; // Average number of people in bathroom at same time
int totalPpl; // Total number of people ever in bathroom
int currentPpl; // Current number of people, if 0 then vacant state

enum gender state; // Is bathroom male or female?

void Initialize()
{
    numUsages = 0;
    totalTime = 0;

    avgQueue = 0;
    totalWaits = 0;

    avgPpl = 0;
    totalPpl = 0;
    currentPpl = 0;

    state = male;
}

void Enter(enum gender g)
{
    // Put a lock here
    if(currentPpl == 0)
    {
        state = g;
        avgPpl++;
        totalPpl++;
        currentPpl++;
        //No people, just enter
    }
    else if(g == state)
    {
        avgPpl++;
        totalPpl++;
        currentPpl++;
        //Same gender, please enter
    }
    else
    {
        //Wrong gender, don't enter
    }
    // Give up lock
}

void Leave(void)
{
    // Lock
    currentPpl--;
    // Release
}

void Finalize()
{
    avgQueue = avgQueue / totalWaits;
    avgPpl = avgPpl / totalPpl; // This math is totally wrong atm

    printf("Statistics of Bathroom:\n");
    printf("Number of times used: %d", numUsages);
    printf("Total time used: %d", totalTime);
    printf("Average time spent waiting: %f", avgQueue);
    printf("Average people in at a time: %f", avgPpl);
}
