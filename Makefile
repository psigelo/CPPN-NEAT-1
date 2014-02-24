EXECUTABLE = prueba_NEAT
VPATH = ./src ./headers ./objects
CC = g++ -O3
CFLAGS = -g -Wall -I./headers -I./objects -I./src

CPPN-NEAT: run_CPPN-NEAT.cpp CPPN-NEAT.o  genetic_encoding.o fitness.o 
	@$(CC) $(CFLAGS) ./objects/function.o ./src/run_CPPN-NEAT.cpp ./objects/CPPN-NEAT.o ./objects/genetic_encoding.o ./objects/fitness.o -o ./executables/CPPN-NEAT

CPPN-NEAT.o: CPPN-NEAT.cpp
	@$(CC) $(CFLAGS) -c ./src/CPPN-NEAT.cpp  -o ./objects/CPPN-NEAT.o

example1.o: example1.cpp example1.hpp 
	@$(CC) $(CFLAGS) -c example1.cpp -o ./objects/example1.o	

genetic_encoding.o: genetic_encoding.cpp function.o
	@$(CC) $(CFLAGS) -c ./src/genetic_encoding.cpp -o ./objects/genetic_encoding.o

fitness.o: fitness.cpp 
	@$(CC) $(CFLAGS) -c ./src/fitness.cpp  -o ./objects/fitness.o

function.o: function.cpp 
	@$(CC) $(CFLAGS) -c ./src/function.cpp  -o ./objects/function.o

clean:
	@rm -f ./objects/*.o ./executables/*
