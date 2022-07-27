WFLAGS=-Wall -Wpedantic -Wextra
OPT=-O3

all : threads.exe

threads.exe: threads.cpp Makefile
	g++ -o threads.exe $(WFLAGS) $(OPT) threads.cpp

clean:
	rm threads.exe
