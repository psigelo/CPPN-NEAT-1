
#ifndef FUNCTION_HH
#define FUNCTION_HH

#include <cmath>
#include <string>
#include <vector>

using namespace std;

namespace ANN_USM
{
	class Function
	{
		public:

			Function();
			Function(string);

			double eval(double);

			string get_name(int function);
			string get_name();

		private:

			string function_name;

			// Useful numbers
			const double Pi;
			const double E;

			// Function Parameters, if any
			vector<double> param;

			// Function pointer
			double (Function::*function)(double);

			// Functions
			double Sin(double);
			double Cos(double);
			double Identity(double);
			double Gaussian(double);
			double Abs(double);
			double Sigmoid(double);
	};
}

#endif