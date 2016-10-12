all : AdaptiveLocalThreshold.o Ideka.o GuoHall.o CrossingNumber.o Filter.o Minutiae.o Functions.o BinarisationTest.cpp FingerPrintProcess
	g++ BinarisationTest.cpp AdaptiveLocalThreshold.o Functions.o Ideka.o GuoHall.o CrossingNumber.o Filter.o Minutiae.o `pkg-config --libs --cflags opencv` -o BinarisationTest

AdaptiveLocalThreshold : AdaptiveLocalThreshold.cpp AdaptiveLocalThreshold.h
	g++ -c AdaptiveLocalThreshold.cpp
CrossingNumber: CrossingNumber.cpp CrossingNumber.h Minutiae.h Minutiae.cpp
	g++ -c CrossingNumber.cpp
Minutiae: Minutiae.h Minutiae.cpp
	g++ -c Minutiae.cpp
Filter: Filter.cpp Filter.h Minutiae.h Minutiae.cpp
	g++ -c Filter.cpp
Ideka: GuoHall.cpp GuoHall.h Ideka.cpp Ideka.h
	g++ -c Ideka.cpp
GuoHall: GuoHall.cpp GuoHall.h
	g++ -c GuoHall.cpp
Functions: Functions.cpp Functions.h
	g++ -c Functions.cpp
FingerPrintProcess: fingerprint_process.cpp
	g++ -o fingerprint fingerprint_process.cpp `pkg-config --libs --cflags opencv`
clean:
	rm *.o BinarisationTest fingerprint