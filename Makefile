# Kyle Savell & Antony Qin

all: clean p3.c
	gcc -Wall -pthread -g p3.c -o p3 -lm

clean:
	rm -f p3
