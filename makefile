# COMP4981 Assignment 2 makefile

CC = g++-6
CFLAGS = -Wall -ggdb -pthread

main: main.o msgqWrapper.o
	$(CC) $(CFLAGS) main.o msgqWrapper.o -o main

clean:
	rm -f *.o *.bak main

main.o: Main.cpp
	$(CC) $(CFLAGS) -c -O Main.cpp

msgqWrapper.o: msgqWrapper.cpp
	$(CC) $(CFLAGS) -c -O msgqWrapper.cpp