# Kyle Savell & Antony Qin

all: bathroomSim.c p3.c
	gcc -Wall -pthread -g p3.c bathroomSim.c bathroom.h -o p3 -lm

clean:
	rm -f bathroomSim
	rm -f p3

