// Kyle Savell & Antony Qin
// Project 3: Bathroom

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

// Struct for input arguments for threads
struct args {
	enum gender person;
	int arrival;
	int stay;
	int loops;
};
// Struct for bathroom
struct monitor {
	int occupants[100];
	enum filled_with status;

	int numUsages;
	float totalTime;
	float avgQueue;
	float avgPpl;
	struct timeval curStart;
	struct timeval curEnd;
};

void Enter(enum gender g); //Enter bathroom, set state if needed
void Leave(void); //Leave bathroom, set state if needed
void Initialize();
void Finalize();

