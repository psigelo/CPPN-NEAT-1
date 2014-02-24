#include "CPPN-NEAT.hpp"
#include "genetic_encoding.hpp"

using namespace std;
using namespace ANN_USM;

#include <ctime>

int main(int argc, char** argv)
{
	srand(time(0)); // use current time as seed for random generator
	
	if(argc < 2){
		cerr << "Arguments missing. The relative path to the genetic encoding file ( *.genetic_encoding ) must be defined!" << endl;
		exit(1);
	}

	Population poblacion;

	poblacion.init_population(argv[1]);
	
	for (int i = 0; i < GENERATIONS; ++i) poblacion.epoch();

	//char path[] = "./files/result_population";
	//poblacion.save(path);

	cout << "champion: " << poblacion.fitness_champion << endl;

	return 0;
}