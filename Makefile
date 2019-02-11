CC=g++
LIBRARY=lib

all : vision

vision : visionTester.cpp
	$(CC) $< -o $@

clean :
	rm -f vision *.o $(LIBRARY)\*.o results/*
