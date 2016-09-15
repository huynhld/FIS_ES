all: main.o synoapi.o
	g++ -Wall  main.o synoapi.o -o main
synoapi: synoapi.cpp synoapi.h utility.h
	g++ -c  synoapi.cpp
main: main.cpp
	g++ -c  main.cpp
clean:
	rm *.o main
