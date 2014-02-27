#ifndef GENETIC_ENCODING_CPP
#define GENETIC_ENCODING_CPP

#include <stdio.h>
#include <cstring>

#include "genetic_encoding.hpp"

using namespace ANN_USM;

//==================================================================
//	Connection Gene
//==================================================================

void connection_gene::c_g(int innovation, int in, int out, double weight, bool enable)
{
	this->innovation 	= innovation;
	this->in 			= in;
	this->out 			= out;
	this->weight 		= weight;
	this->enable 		= enable;
	this->exist 		= true;
}

void connection_gene::kill()
{
	this->exist = false;
}

//==================================================================
//	Node Gene
//==================================================================

node_gene::node_gene()
{
	this->incoming_connections = 0;
	this->accumulative_result = 0;
	this->counter = 0;
	this->final_result = 0;
}

void node_gene::n_g(int node, int row, gene_type type, string function)
{
	this->node 		= node;
	this->type 		= type;
	this->row		= row;
	this->exist 	= true;
	this->function 	= new Function(function);
}

void node_gene::kill()
{
	this->exist = false;
}

double node_gene::get_final_result()
{
	return this->final_result;
}

bool node_gene::is_ready()
{
	return this->incoming_connections == this->counter;
}

void node_gene::increase_incoming_connection()
{
	this->incoming_connections++;
}

void node_gene::decrease_incoming_connection()
{
	this->incoming_connections--;
}

void node_gene::eval(double value)
{
	this->accumulative_result += value;
	this->counter++;
	
	if(this->is_ready())
	{
		this->final_result = this->function->eval(this->accumulative_result);
		this->accumulative_result = 0;
	}
}

//==================================================================
//	Gene Encoding
//==================================================================

//==================================
//	Add Connection

void Genetic_Encoding::add_connection(connection_gene conn)
{
	this->add_connection(conn.innovation, conn.in, conn.out, conn.weight, conn.enable);
}

void Genetic_Encoding::add_connection(int innovation, int in, int out, double weight, bool enable)
{
	int list_size = Lconnection_genes.size();

	// If the connection's innovation number to be created is greater than the current one, fill the
	// connection's vector with empty connection (exist = 0) until the desired innovation number is reached.
	if(innovation >= list_size)
	{
		connection_gene missing_connection_gene;
		connection_gene new_connection;

		missing_connection_gene.kill();

		new_connection.c_g(innovation, in, out, weight, enable);

		for (int i = 0; i < innovation - list_size; ++i)
		{
			Lconnection_genes.push_back(missing_connection_gene);
		}
		Lconnection_genes.push_back(new_connection);
	}

	// If it is within the list, check if it exists or not.
	else 
	{
		if(Lconnection_genes.at(innovation).exist)
			cerr << "ERROR::In function add_connection, you wanted to add a connection gene with an innovation that already exists" << endl;	
		else
			Lconnection_genes.at(innovation).c_g(innovation, in, out, weight, enable);
	}

	if(enable == true)
	{
		Lnode_genes.at(in).outgoing_connections.push_back(innovation);
		Lnode_genes.at(out).increase_incoming_connection();
	}
}

//==================================
//	Add Node

void Genetic_Encoding::add_node(node_gene node)
{
	this->add_node(node.node, node.row, node.type, node.function->get_name());
}

void Genetic_Encoding::add_node(int node, int row , gene_type type, string function)
{
	int list_size = Lnode_genes.size();

	// If the node to be created is greater than the current one, fill the
	// node's vector with empty nodes (exist = 0) until the desired node is reached.
	if(node >= list_size)
	{
		node_gene missing_node_gene;
		node_gene new_node;

		missing_node_gene.kill();

		new_node.n_g(node, row, type, function);

		for (int i = 0; i < node - list_size; ++i)
		{
			Lnode_genes.push_back(missing_node_gene);
		}
		Lnode_genes.push_back(new_node);
	}

	// If it is within the list, check if it exists or not.
	else
	{
		if(Lnode_genes.at(node).exist)
			cerr << "warning: at Genetic_Encoding::add_node an existing node was tried to be added again!" << endl;	
		else
			Lnode_genes.at(node).n_g(node, row, type, function);
	}

	if(type == INPUT)
	{
		this->Lnode_genes.at(node).increase_incoming_connection();
		this->input_nodes.push_back(node);
	}
	else if(type == OUTPUT) this->output_nodes.push_back(node);
}

//==================================
//	Evaluation of the Network

vector <double> Genetic_Encoding::eval(vector<double> input_values)
{
	vector<double> outputs;

	if(input_values.size() != this->input_nodes.size())
	{
		cerr << "error in function 'Genome::eval'. Number of input values differ from number of input nodes." << endl;
	}
	else
	{
		// Spread the initial values through the network
		for (int i = 0; i < (int)this->input_nodes.size(); i++)
			this->spread_final_result(this->input_nodes.at(i), input_values.at(i));

		// Recollect all the final results from the output nodes
		for (int i = 0; i < (int)this->output_nodes.size(); i++)
			outputs.push_back(this->Lnode_genes.at(this->output_nodes.at(i)).get_final_result());			
	}

	return outputs;
}

// Verificar
/*vector<connection_gene> Genetic_Encoding::get_outgoing_connections(int node)
{
	vector<connection_gene> outgoing_connections;

	for (int i = 0; i < (int)this->Lconnection_genes.size(); i++)
	{
		if(	this->Lconnection_genes.at(i).exist && \
			this->Lconnection_genes.at(i).enable && \
			this->Lconnection_genes.at(i).in == node)
			outgoing_connections.push_back(this->Lconnection_genes.at(i));
	}
	
	return outgoing_connections;
}*/

void Genetic_Encoding::spread_final_result(int node, double value)
{
	// Evaluates the node
	this->Lnode_genes.at(node).eval(value);

	if(this->Lnode_genes.at(node).is_ready())
	{
		this->Lnode_genes.at(node).counter = 0;

		value = this->Lnode_genes.at(node).get_final_result();

		//vector<connection_gene> outgoing_connections = this->get_outgoing_connections(node);

		for (int i = 0; i < (int)Lnode_genes.at(node).outgoing_connections.size(); i++)
			this->spread_final_result(Lconnection_genes.at(Lnode_genes.at(node).outgoing_connections.at(i)).out, value * Lconnection_genes.at(Lnode_genes.at(node).outgoing_connections.at(i)).weight);
			//this->spread_final_result(outgoing_connections.at(i).out, value * outgoing_connections.at(i).weight);
	}
}

//==================================
//	Stream & Misc

string Genetic_Encoding::JSON()
{
	stringstream o;

	o << "{\n\t\"Genetic_Encoding\":\n\t{\n\t\t\"nodes\":\n\t\t[\n";

	for (int i = 0; i < (int)Lnode_genes.size(); ++i)
	{
		o << "\t\t\t{\"exist\": " << Lnode_genes[i].exist;

		if(Lnode_genes.at(i).exist) 
		{
			o << ",\"node\": " <<Lnode_genes.at(i).node;
			o << ",\"type\": " << Lnode_genes.at(i).type;
			o << ", \"row\": " << Lnode_genes.at(i).row;
			o << ", \"function\": " << Lnode_genes.at(i).function->get_name();
		}

		o << ((i < (int)Lnode_genes.size() - 1) ? "},\n" : "}\n");
	}

	o << "\t\t],\n\t\t\"connection_genes\":\n\t\t[\n";

	for (int i = 0; i < (int)Lconnection_genes.size(); ++i)
	{
		o << "\t\t\t{\"exist\": " << Lconnection_genes[i].exist;

		if(Lconnection_genes[i].exist)
		{ 
			o << ",\"innovation\": " << Lconnection_genes.at(i).innovation;
			o << ",\"in\": " << Lconnection_genes.at(i).in;
			o << ",\"out\": " << Lconnection_genes.at(i).out;
			o << ",\"weight\": " << Lconnection_genes.at(i).weight;
			o << ",\"enable\": " << Lconnection_genes.at(i).enable;
		}
		
		o << ((i < (int)Lconnection_genes.size() - 1) ? "},\n" : "}\n");
	}
	
	o << "\t\t]\n\t}\n}";

	return o.str();
}

void Genetic_Encoding::save(char path[])
{
	ofstream file;
	file.open (path);
	file << JSON();
	file.close();
}

void Genetic_Encoding::load(char path[])
{
	Lconnection_genes.clear(); 
	Lnode_genes.clear();

	ifstream file (path);
	file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
	char buffer[length]; // In JSON format
	file.read (buffer,length);
	file.close();
	
	int node;
	int type;
	int innovation;
	int in;
	int out;
	int enable;
	int contador(0);
	int row;

	char * pch;
	char delimiters[] = " \n\":\t{},[";

	bool connection(false);
	bool exist;
//==================================
//	Stream & Misc
	double weight;

	pch = strtok (buffer,delimiters);
	
	do
	{
		pch = strtok (NULL, delimiters);
		if(!(pch[0] == ']')){
			if(connection){
				if(!strncmp(pch, "exist",5)){
					pch = strtok (NULL, delimiters);
					exist = atoi(pch);
					if(exist){
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						innovation = atoi(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						in = atoi(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						out = atoi(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						weight = (double)atof(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						enable = atoi(pch);
						this->add_connection(innovation, in, out, weight, (bool)enable);
					}
				} 
			}
			else{
				if(!strncmp(pch, "exist",5)){
					pch = strtok (NULL, delimiters);
					exist = atoi(pch);
					if(exist){
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						node = atoi(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						type = atoi(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						row = atoi(pch);
						pch = strtok (NULL, delimiters);
						pch = strtok (NULL, delimiters);
						this->add_node(node, row, (gene_type) type, string(pch));
					}
				} 
			}
		}
		else
		{
			contador++;
			connection = true;
			if(contador == 2) break;
		}
	}
	while (pch != NULL);


}

ostream & operator<<(ostream & o, Genetic_Encoding & encoding) 
{
	o << encoding.JSON();	
	return o;
}

#endif