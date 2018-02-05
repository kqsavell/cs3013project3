# Kyle Savell & Antony Qin

all: clean bathroomSim.c
	gcc -Wall -pthread -g bathroomSim.c bathroom.h -o bathroomSim -lm

clean:
	rm -f bathroomSim
