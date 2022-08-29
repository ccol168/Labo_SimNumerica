CC = g++
CFLAGS = -Wall -O3 --std=c++11


mainmpi.exe : mainmpi.o random.o functions.o point.o path.o generation.o
	mpicxx random.o mainmpi.o functions.o point.o path.o generation.o -o mainmpi.exe
mainmpi.o : mainmpi.cpp path.h point.h random.h functions.h
	mpicxx -c mainmpi.cpp -o mainmpi.o $(CFLAGS)
main.exe : main.o random.o functions.o point.o path.o generation.o
	$(CC) random.o main.o functions.o point.o path.o generation.o -o main.exe
main.o : main.cpp path.h point.h random.h functions.h
	$(CC) -c main.cpp -o main.o $(CFLAGS)
generation.o: generation.cpp generation.h random.h point.h path.h
	$(CC) -c generation.cpp -o generation.o $(CFLAGS)
path.o : path.cpp path.h random.h 
	$(CC) -c path.cpp -o path.o $(CFLAGS)
point.o : point.cpp point.h
	$(CC) -c point.cpp -o point.o $(CFLAGS)
random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)
functions.o : functions.h functions.cpp
	$(CC) -c functions.cpp -o functions.o $(CFLAGS)
clean :
	rm *.o main.exe seed.out
