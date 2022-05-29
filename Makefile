WARNINGS = -Wall -Wpedantic -Wextra

all: parallel_max.exe

parallel_max.exe: parallel_max.cpp Makefile
	g++ -o $@ -std=c++17 $(WARNINGS) -O3 parallel_max.cpp

clean:
	rm parallel_max.exe
