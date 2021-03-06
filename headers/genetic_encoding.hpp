#ifndef GENETIC_ENCODING_hPP
#define GENETIC_ENCODING_hPP


#include <vector>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <strings.h>

#include "user_definitions.hpp"
#include "function.hpp"

using namespace std;

namespace ANN_USM
{
	enum gene_type
	{
		INPUT,
		HIDDEN,
		OUTPUT
	};

	//==================================================================
	//	Connection Gene
	//==================================================================

	class connection_gene
	{
		public:

			void c_g(int innovation, int in, int out, double weight, bool enable); // fill a conection gene
			void kill();

			int innovation;
			int in;
			int out;
			bool enable;
			double weight;
			bool exist; 
	};

	//==================================================================
	//	Node Gene
	//==================================================================

	class node_gene
	{
		public:

			node_gene();

			void n_g(int node, int row, gene_type type, string function); // fill a node gene
			void kill();
			void increase_incoming_connection();
			void decrease_incoming_connection();
			void eval(double value);

			double get_final_result();

			bool is_ready();

			int row;
			int node;
			int incoming_connections;
			int counter;

			bool exist;

			double accumulative_result;
			double final_result;
			
			gene_type type;

			Function * function;
	};

	//==================================================================
	//	Gene Encoding
	//==================================================================

	class Genetic_Encoding
	{
		public:

			void add_node(int node, int row, gene_type type, string function);
			void add_node(node_gene node);
			void add_connection(int innovation, int in, int out, double weight, bool enable);
			void add_connection(connection_gene conn);
			void change_weight(int innovation, double weight);
			void save(char path[]); // save to a file
			void load(char path[]); // Load an genetic_encoding from a file.
			void spread_final_result(int node, double value);

			string JSON();

			vector<connection_gene> get_outgoing_connections(int node);
			
			vector<double> eval(vector <double> inputs);

			vector<connection_gene> Lconnection_genes; //List of connections genes
			vector<node_gene> Lnode_genes;

			vector<int> input_nodes;			
			vector<int> output_nodes;	
			vector<int> row_orderer_list;

			int niche;

			double fitness;
	};
}

ostream & operator<<(ostream & o, ANN_USM::Genetic_Encoding & encoding);

#endif