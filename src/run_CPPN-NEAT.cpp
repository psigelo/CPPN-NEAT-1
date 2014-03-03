
#include <ctime>
#include <cstring>
#include <string>

#include "CPPN-NEAT.hpp"
#include "genetic_encoding.hpp"

using namespace std;
using namespace ANN_USM;

void print_usage(string proc)
{
	cerr << "Usage:" << endl << "\t" << proc << " -t file_name.genome_encoding -x file_name.genetic_encoding -v value_1 value_2 ..." << endl << endl;
	cerr << "\t-t file_name\t\tTrain the network encoded within 'file_name'" << endl;
	cerr << "\t-x file_name\t\tExecute the network encoded within 'file_name'" << endl;
	cerr << "\t-v value ...\t\tValues used to run the network (only used with '-x')" << endl << endl;
}

int main(int argc, char** argv)
{	
	int exec_file = 0;
	int train_file = 0;
	bool print_champion = false;

	vector<double> inputs;
	vector<double> outputs;

	//===============
	// Various checks
	//===============

	if(argc == 1)
	{
		cerr << "error: missing flag!" << endl;
		print_usage(argv[0]);
		return -1;	
	}

	for (int i = 1; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-t"))
		{
			if(++i < argc) train_file = i;
			else
			{
				cerr << "error: missing input file name after -t flag!" << endl;
				print_usage(argv[0]);
				return -1;
			}
		}
		else if(!strcmp(argv[i], "-x"))
		{
			if(++i < argc) exec_file = i;
			else
			{
				cerr << "error: missing input file name after -x flag!" << endl;
				print_usage(argv[0]);
				return -1;
			}
		}
		else if(!strcmp(argv[i], "-v"))
		{
			if(++i < argc)
				while(i < argc)
					inputs.push_back(atoi(argv[i++]));
			else
			{
				cerr << "error: missing input values after -v flag!" << endl;
				print_usage(argv[0]);
				return -1;
			}
		}
		else if(!strcmp(argv[i], "-p"))
		{
			print_champion = true;
		}
		else if(!strcmp(argv[i], "-h"))
		{
			print_usage(argv[0]);
			return 0;
		}
		else
		{
			cerr << "error: invalid flag '" << argv[i] << "'!" << endl;
			print_usage(argv[0]);
			return -1;
		}
	}

	if(exec_file != 0 && inputs.size() == 0)
	{
		cerr << "error: missing values!" << endl;
		print_usage(argv[0]);
		return -1;
	}

	if(exec_file == 0 && inputs.size() != 0)
	{
		cerr << "warning: values were omitted!" << endl;
	}

	//===============
	// Training
	//===============

	if(train_file != 0)
	{
		srand(time(0)); // use current time as seed for random generator

		Population population(argv[train_file]);
		
		for (int i = 0; i < GENERATIONS; ++i) population.epoch();

		string output_file(argv[train_file]);

		if(output_file.substr(output_file.size() - 17, 17) == ".genetic_encoding")
			output_file.insert(output_file.size() - 17, ".out");	
		else
			output_file += ".out.genetic_encoding";	

		//==================================================
		// Save only the champion organism

			population.save((char*)output_file.c_str());

		//==================================================
		// Save all the organism created along the training

			// population.save_all(output_file);

		//==================================================
		
		if(print_champion == true)
		{
			cout << endl;
			cout << "CHAMPION FITNESS: " << endl;
			cout << "-----------------" << endl << endl;
			cout << "\t" << population.fitness_champion << endl << endl;

			cout << "CHAMPION NETWORK:" << endl;
			cout << "-----------------" << endl << endl;
			cout << population.champion << endl;
		}		
	}

	//===============
	// Executing
	//===============

	if(exec_file != 0)
	{
		Genetic_Encoding genome;

		genome.load(argv[exec_file]);

		genome.eval(inputs);
		outputs = genome.eval(inputs);

		cout << "Output: [ ";
		for (int i = 0; i < (int)outputs.size(); ++i)
			cout <<  outputs.at(i) << ((i < (int)outputs.size() - 1) ? " , " : " " );
		cout << "]" << endl;
	}

	return 0;
}