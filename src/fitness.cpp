#ifndef FITNESS_CPP
#define FITNESS_CPP

#include "fitness.hpp"
#include <stdlib.h>
#include <cmath>

double fitness(Genetic_Encoding organism)
{
	vector<double> output;
	vector<double> input;

	double error_sum = 0;

	//======================
	//	XOR
	//======================

	// XOR(0,0) -> 0
	input.push_back(0);
	input.push_back(0);
	output = organism.eval(input);
	error_sum += abs(output.at(0));
	input.clear();

	// XOR(0,1) -> 1
	input.push_back(0);
	input.push_back(1);
	output = organism.eval(input);
	error_sum += 1 - output.at(0);
	input.clear();

	// XOR(1,0) -> 1	
	input.push_back(1);
	input.push_back(0);
	output = organism.eval(input);
	error_sum += 1 - output.at(0);
	input.clear();

	// XOR(1,1) -> 0
	input.push_back(1);
	input.push_back(1);
	output = organism.eval(input);
	error_sum += abs(output.at(0));
	input.clear();

	// Fitness
	double fitness;
	fitness = pow(4 - error_sum, 2);

	cout << "fitness:\t" << fitness << endl;

	return fitness;
}

#endif