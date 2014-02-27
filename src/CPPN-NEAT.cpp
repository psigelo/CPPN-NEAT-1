
#ifndef CPPN_NEAT_CPP
#define CPPN_NEAT_CPP

#include "CPPN-NEAT.hpp"

using namespace ANN_USM;

//===================================================================================
//	Population
//===================================================================================

Population::Population(char path[])
{
	Genetic_Encoding organism;

	organism.load(path);

	organism.niche = 0;
	
	row_orderer_list.push_back(0);
	row_orderer_list.push_back(1);

	last_row = 1;
	fitness_champion = 0;
	champion = organism;
	last_innovation = 0;

	for (int i = 0; i < (int)organism.Lconnection_genes.size(); ++i)
		obtain_innovation(organism.Lconnection_genes.at(i).in, organism.Lconnection_genes.at(i).out); 

	last_node = (int)organism.Lnode_genes.size() - 1;

	for (int i = 0; i < POPULATION_MAX; ++i)
		organisms.push_back(mutation_node(put_randoms_weight(organism))); 

	lenght = POPULATION_MAX;

	prev_organisms.push_back(organism);
	Niche niche_temp;
	niche_temp.organism_position.push_back(0);
	niche_temp.exist = true;
	niche_temp.niche_champion_position = 0;
	current_niches.push_back(niche_temp);

	spatiation();
}

Genetic_Encoding Population::put_randoms_weight(Genetic_Encoding organism)
{
	for (int i = 0; i < (int)organism.Lconnection_genes.size(); ++i)
		organism.Lconnection_genes.at(i).weight = 2.0 * (rand() % 10000) / 10000.0 - 1.0;

	return organism;
}

Genetic_Encoding Population::mutation_change_weight(Genetic_Encoding organism)
{
	int connection_to_mutate = round(rand() % int(organism.Lconnection_genes.size()));

	double delta = 2 * (rand() % 10000) / 10000.0 - 1;

	organism.Lconnection_genes.at(connection_to_mutate).weight = delta * 0.3 + 0.7 * organism.Lconnection_genes.at(connection_to_mutate).weight;

	return organism;
}

Genetic_Encoding Population::mutation_node(Genetic_Encoding organism)
{
	int number_of_connections = organism.Lconnection_genes.size();
	int connection_to_mutate;
	int innov1;
	int innov2;
	int node;
	int count(0);
	int row;

	do
	{
		while(true)
		{
			connection_to_mutate = round(rand()%number_of_connections);
			if(organism.Lconnection_genes[connection_to_mutate].exist) break;
		}
		
		node = obtain_historical_node(organism.Lconnection_genes[connection_to_mutate].in, organism.Lconnection_genes[connection_to_mutate].out);

		if(node < (int)organism.Lnode_genes.size())
		{
			if(!organism.Lnode_genes[node].exist)
			{
				break;
			}
		}
		else break;
		
		if(count++ > 50)
		{
			cerr << "In function Mutation_node:: in 50 attempts not found an mutation option";
			return organism;
		}

	}
	while(true);

	row = obtain_row(node, organism.Lnode_genes[organism.Lconnection_genes[connection_to_mutate].in].row, organism.Lnode_genes[ organism.Lconnection_genes[connection_to_mutate].out ].row );

	Function function;
	organism.add_node(node, row , HIDDEN, function.get_name((int)round(rand() % FUNCTION_NUM)));
	
	// add connections
	organism.Lconnection_genes[connection_to_mutate].enable = 0; // disabling old connection.
	
	organism.Lnode_genes.at(organism.Lconnection_genes[connection_to_mutate].out).decrease_incoming_connection();

	innov1 = obtain_innovation(organism.Lconnection_genes[connection_to_mutate].in, node);
	innov2 = obtain_innovation(node, organism.Lconnection_genes[connection_to_mutate].out);
	
	organism.add_connection(innov1, organism.Lconnection_genes[connection_to_mutate].in, node, 1.0, true);
	organism.add_connection(innov2, node, organism.Lconnection_genes[connection_to_mutate].out, organism.Lconnection_genes[connection_to_mutate].weight, true);
	
	return organism;
}

// Hay que mejorar esto
int Population::obtain_row(int node, int row_node_initial_in, int row_node_initial_out)
{	
	int row_position_in(-1);
	int row_position_out(-1);

	bool flag_in(false), flag_out(false);

	for (int i = 0; i < (int)row_orderer_list.size(); ++i)
	{
		if(!flag_in && row_node_initial_in == row_orderer_list.at(i)) 
		{  
			row_position_in = i; 
			flag_in = true; 
		}

		if(!flag_out && row_node_initial_out == row_orderer_list.at(i))
		{  
			row_position_out = i; 
			flag_out = true;
		}

		if(flag_in && flag_out) break;
	}
	
	if(row_position_in == -1 || row_position_out == -1 )
	{
		cerr << row_node_initial_in << "\t" << row_position_in << "\t" << row_node_initial_out << "\t" << row_position_out << "\t" << row_orderer_list.size() << "\n" << "Error:: Function obtain_row :: Row_node_in or row_node_out does not exist .\n"; exit(1);
	}

	if( row_position_in > row_position_out)
	{
		cerr << row_node_initial_in << "\t" << row_position_in << "\t" << row_node_initial_out << "\t" << row_position_out << "\t" << row_orderer_list.size() << "\n"; 
		cerr << "Error:: Function obtain_row :: ------------------------.\n"; exit(1);
	}

	while((int)historical_row.size()-1 < node)
	{
		historical_row.push_back(-1);
	}

	if(historical_row[node] >= 0) 
	{
		return historical_row[node];
	}
	
	if(row_position_in == row_position_out)
	{
		return row_node_initial_in;
	}
	else
	{
		if(row_position_out - row_position_in == 1)
		{
			historical_row.at(node) = ++last_row;
			row_orderer_list.insert(row_orderer_list.begin() + row_position_in + 1, last_row);

			return historical_row.at(node);
		}
		else
		{
			historical_row.at(node) = row_orderer_list[row_position_in + 1];

			return historical_row.at(node);
		}
	}	
}

int Population::obtain_historical_node(int in, int out)
{
	while((int)historical_nodes.size() - 1 < in)
		historical_nodes.push_back(vector<int> ());

	while((int)historical_nodes.at(in).size() - 1 < out)
		historical_nodes.at(in).push_back(-1);

	if(historical_nodes.at(in).at(out) < 0)
		historical_nodes.at(in).at(out) = ++last_node;

	return historical_nodes.at(in).at(out);
}

/*! \brief	Obtain the (historical) innovation number between two nodes.
*			
*	If both nodes were connected at any time in the past then they will
*	have the very same innovation number they had earlier. 
*
*	If they weren't connected at all then they will have a new innovation
*	number, filling every vector in between with -1, i.e. not connected.
*/
int Population::obtain_innovation(int in, int out)
{
	/*!	If the inner node of the connection is greater than the vector size,
	*	fill with empty vectors until reach the desired node.
	*/
	while((int)historical_innovation.size() - 1 < in)
		historical_innovation.push_back(vector<int> ());

	/*!	If the outgoing node of the connection is greater than the vector size
	*	at the inner position, fill with -1 until reach the desired node.
	*/
	while((int)historical_innovation.at(in).size() - 1 < out)
		historical_innovation.at(in).push_back(-1);

	/*!	If the desired pair of nodes was not connected in the past, give the new
	*	innovation number to the connection and increase in one the innovation number.
	*	
	*	If it was connected then skip the if statement and return the innovation number
	*	of the pair of nodes.
	*/
	if(historical_innovation.at(in).at(out) < 0)
		historical_innovation.at(in).at(out) = last_innovation++;

	return historical_innovation.at(in).at(out);
}

Genetic_Encoding Population::mutation_connection(Genetic_Encoding organism)
{
	int count(0), counter(0);
	int node_in;
	int node_out;
	int innovation;
	int number_of_nodes = (int)organism.Lnode_genes.size();
	
	while(true)
	{
		counter = 0;
		while(true)
		{
			node_in = round(rand()%number_of_nodes);
			node_out = round(rand()%number_of_nodes);
			//asdlfasjdfpoasijfpdoiasj

			int row_position_in(-1);
			int row_position_out(-1);
			bool flag_in(false), flag_out(false);
			
			for (int i = 0; i < (int)row_orderer_list.size(); ++i)
			{
				if(organism.Lnode_genes[node_in].row == row_orderer_list.at(i))
				{   
					row_position_in = i; 
					flag_in = true;
				}

				if(organism.Lnode_genes[node_out].row == row_orderer_list.at(i))
				{ 	
					row_position_out = i; 
					flag_out = true;
				}

				if(flag_in && flag_out) break;
			}
	
			if((organism.Lnode_genes[node_out].exist && organism.Lnode_genes[node_in].exist) && ( row_position_in <  row_position_out)) break;

			if (counter > 50)
			{			
				//cerr << "In function mutation_connection:: counter is greater than 50 \n";
				return organism;
			}

			counter++;
		}
		
		innovation = obtain_innovation(node_in, node_out);

		if((int)organism.Lconnection_genes.size() - 1 < innovation)
		{
			organism.add_connection(innovation,node_in, node_out, 2*(rand()%10000)/10000.0 - 1.0, true);
			break;
		}
		else if(!organism.Lconnection_genes[innovation].exist)
		{
			organism.add_connection(innovation,node_in, node_out, 2*(rand()%10000)/10000.0 - 1.0, true);
			break;
		}

		if(count>50)
		{
			//cerr << "In function mutation_connection:: in 50 attempts not found an mutation option \n";
			break;
		}

		count++;
	}	

	return organism;
}

ostream & operator<<(ostream & o, ANN_USM::Population & pop) 
{ 
	for (int i = 0; i < pop.lenght; ++i)
	{
		o << pop.organisms.at(i);
		o << "\n";
	}

	return o;
}

void Population::save_all(char path[])
{
	ofstream file;
	file.open (path);
	file << *this;
	file.close();
}

void Population::save(char path[])
{
	this->champion.save(path);
}

/*!	\brief	Measure the distance between two organism. 
*/
double Population::compatibility(Genetic_Encoding orgm1, Genetic_Encoding orgm2)
{
	int E = 0;
	int D = 0;
	int size_1 = orgm1.Lconnection_genes.size();
	int size_2 = orgm2.Lconnection_genes.size();
	int limit_sup;
	int limit_inf;

	double W = 0.;

	limit_sup = (size_1 > size_2) ? size_2 : size_1;
	limit_inf = (size_1 < size_2) ? size_1 : size_2;

	for (int i = 0; i < limit_inf; ++i)
	{
		if(orgm1.Lconnection_genes.at(i).exist && orgm2.Lconnection_genes.at(i).exist)
			W += abs(orgm1.Lconnection_genes.at(i).weight - orgm2.Lconnection_genes.at(i).weight);
		else if(orgm1.Lconnection_genes.at(i).exist || orgm2.Lconnection_genes.at(i).exist)
			D++;
	}
		
	for (int i = limit_inf; i < limit_sup; ++i)
		if((size_1 < size_2) ? orgm2.Lconnection_genes.at(i).exist : orgm1.Lconnection_genes.at(i).exist) 
			E++;

	return DISTANCE_CONST_1 * E / DISTANCE_CONST_4 + DISTANCE_CONST_2 * D / DISTANCE_CONST_4 + DISTANCE_CONST_3 * W;
}

Genetic_Encoding Population::crossover(Genetic_Encoding orgm1, Genetic_Encoding orgm2)
{
	Genetic_Encoding orgm_resutl;

	int conn_size_1 = orgm1.Lconnection_genes.size();
	int conn_size_2 = orgm2.Lconnection_genes.size();
	int node_size_1 = orgm1.Lnode_genes.size();
	int node_size_2 = orgm2.Lnode_genes.size();

	int conn_limit_sup, conn_limit_inf, node_limit_sup, node_limit_inf;
	bool connection_1_is_larger, node_1_is_larger;

	if(conn_size_1>conn_size_2) 
	{		
		conn_limit_sup=conn_size_1;		
		conn_limit_inf=conn_size_2;		
		connection_1_is_larger = true;
	}
	else
	{		
		conn_limit_sup=conn_size_2;		
		conn_limit_inf=conn_size_1;		
		connection_1_is_larger = false;	
	}

	if(node_size_1>node_size_2) 
	{		
		node_limit_sup = node_size_1;		
		node_limit_inf = node_size_2;		
		node_1_is_larger = true;	
	}
	else
	{			
		node_limit_sup = node_size_2;		
		node_limit_inf = node_size_1;		
		node_1_is_larger = false;	
	}

	int i=0;
	while(i < node_limit_sup)
	{
		if (i >= node_limit_inf)
		{
			if(node_1_is_larger)
			{
				if(orgm1.Lnode_genes.at(i).exist == 1)
				{
					orgm_resutl.add_node(orgm1.Lnode_genes.at(i));
				}
			}
			else
			{
				if(orgm2.Lnode_genes.at(i).exist == 1)
				{
					orgm_resutl.add_node(orgm2.Lnode_genes.at(i));
				}
			}
		}
		else
		{
			if( orgm1.Lnode_genes.at(i).exist && orgm2.Lnode_genes.at(i).exist)
				(rand()%10 >= 5) ? orgm_resutl.add_node(orgm1.Lnode_genes.at(i)) : orgm_resutl.add_node(orgm2.Lnode_genes.at(i));
			else if ( orgm1.Lnode_genes.at(i).exist || orgm2.Lnode_genes.at(i).exist)
				(orgm1.Lnode_genes.at(i).exist) ? orgm_resutl.add_node(orgm1.Lnode_genes.at(i)) : orgm_resutl.add_node(orgm2.Lnode_genes.at(i));
		}

		i++;
	}
	
	i=0;
	while( i < conn_limit_sup)
	{
		if (i >= conn_limit_inf)
		{
			if(connection_1_is_larger){
				if(orgm1.Lconnection_genes.at(i).exist==1){
					orgm_resutl.add_connection(orgm1.Lconnection_genes.at(i));
				}
			}
			else{
				if(orgm2.Lconnection_genes.at(i).exist==1){
					orgm_resutl.add_connection(orgm2.Lconnection_genes.at(i));
				}
			}
		}

		else{
			if( orgm1.Lconnection_genes.at(i).exist && orgm2.Lconnection_genes.at(i).exist )
				(rand()%10 >= 5) ? orgm_resutl.add_connection(orgm1.Lconnection_genes.at(i)) : orgm_resutl.add_connection(orgm2.Lconnection_genes.at(i));
			else if ( orgm1.Lconnection_genes.at(i).exist || orgm2.Lconnection_genes.at(i).exist)
				(orgm1.Lconnection_genes.at(i).exist ) ? orgm_resutl.add_connection(orgm1.Lconnection_genes.at(i)) : orgm_resutl.add_connection(orgm2.Lconnection_genes.at(i));
		}
		
		i++;
	}

	return orgm_resutl;
}

void Population::spatiation()
{
	vector<Niche> ().swap(prev_niches);
	prev_niches = current_niches;
	vector<Niche> ().swap(current_niches);

	vector<Niche> real_niches;

	for (int i = 0; i < (int)prev_niches.size(); ++i)
		if (prev_niches.at(i).exist)
			real_niches.push_back(prev_niches.at(i));

	Niche aux_niche;
	aux_niche.exist = false;

	for(int i = 0; i < (int)real_niches.size(); i++)
		current_niches.push_back(aux_niche);

	bool have_niche;
	int amount_of_new_niches = 0;

	for(int j = 0; j < (int)organisms.size(); j++)
	{
		have_niche = false;

		for (int i = 0; i < (int)real_niches.size(); ++i)
			if(compatibility(organisms.at(j), prev_organisms.at(real_niches.at(i).niche_champion_position)) < DISTANCE_THRESHOLD)
			{
				have_niche = true;
				current_niches.at(i).exist = true;
				current_niches.at(i).organism_position.push_back(j);
				current_niches.at(i).niche_champion_position = j; // this is temporal until in function epoch the real champion is decided respect its fitness

				break;
			}

		if(!have_niche)
		{
			for(int i = 0; i < amount_of_new_niches; ++i)
			{
				if(compatibility(organisms.at(j), organisms.at(current_niches.at(i + (int)real_niches.size()).niche_champion_position)) < DISTANCE_THRESHOLD)
				{
					have_niche = true;
					current_niches.at(i + (int)real_niches.size()).exist = true;
					current_niches.at(i + (int)real_niches.size()).organism_position.push_back(j);
					current_niches.at(i + (int)real_niches.size()).niche_champion_position = j; // this is temporal until in function epoch the real champion is decided respect its fitness

					break;
				}
			}

			Niche aux2_niche;
			aux2_niche.exist = true;
			aux2_niche.niche_champion_position = j;
			aux2_niche.organism_position.push_back(j);
			current_niches.push_back(aux2_niche);
			amount_of_new_niches++;
		}	
	}
}

void Population::epoch()
{
	int mutation_amount; 
	int random_organism;
	int random_father;
	int random_mother; // for mating
	int random_niche_father;
	int random_niche_mother;

	Genetic_Encoding organism_temp;
	Genetic_Encoding organism_father;
	Genetic_Encoding organism_mother;// for mating

	double total_shared_fitness_population(0.0);
	double fitness_temp;
	double fitness_max(0.0);

	vector <Niche> real_niches;

	for (int i = 0; i < (int)current_niches.size(); ++i)
		if(current_niches.at(i).exist)
			real_niches.push_back(current_niches.at(i));

	// Falta eliminar a los más malos de cada generación
	for (int i = 0; i < (int)current_niches.size(); ++i)
	{
		if(current_niches.at(i).exist)
		{
			current_niches.at(i).total_fitness=0;
			for (int j = 0; j < (int)current_niches.at(i).organism_position.size(); ++j)
			{
				fitness_temp = fitness(organisms[current_niches.at(i).organism_position.at(j)]);

				if(j==0 || fitness_temp > fitness_max)
				{
					fitness_max=fitness_temp;
					current_niches.at(i).niche_champion_position = j;

					if(fitness_temp > fitness_champion)
					{ // Champion over all generations.
						champion = organisms[current_niches.at(i).organism_position.at(j)];
						fitness_champion=fitness_temp;
					}
				}
				current_niches.at(i).total_fitness+=fitness_temp;				
			}

			total_shared_fitness_population+= current_niches.at(i).total_fitness/current_niches.at(i).organism_position.size();
		}
	}


	vector < Genetic_Encoding >().swap(prev_organisms);
	prev_organisms = organisms;
	vector < Genetic_Encoding >().swap(organisms);

	// CAMBIARLO Y USAR REAL_NICHES.
	for (int i = 0; i < (int)current_niches.size(); ++i)
	{
		if(current_niches.at(i).exist)
		{
			current_niches.at(i).amount_of_offspring = round( 	POPULATION_MAX * (current_niches.at(i).total_fitness / current_niches.at(i).organism_position.size()) / total_shared_fitness_population );

			for (int j = 0; j < current_niches.at(i).amount_of_offspring; ++j)
			{
				if(j == 0)
				{ // all niche champions pass throgh generations.
					organisms.push_back(prev_organisms[current_niches.at(i).niche_champion_position]);
				}
				if(rand()%100 < PERCENTAGE_OFFSPRING_WITHOUT_CROSSOVER)
				{
					random_organism = rand()%current_niches.at(i).organism_position.size();
					organism_temp = prev_organisms[random_organism];
					mutation_amount = rand()%(int)round(prev_organisms[random_organism].Lconnection_genes.size()*PERCENT_MUTATION_CONNECTION) + 1; 

					for(int k = 0; k < mutation_amount; k++)
						organism_temp = mutation_change_weight(organism_temp);
				}
				else
				{
					if((rand()%1000)/1000.0 < PROBABILITY_INTERSPACIES_MATING)
					{
						while(true)
						{
							random_niche_father = rand()%real_niches.size();
							random_niche_mother = rand()%real_niches.size();

							if(random_niche_mother != random_niche_father) break;

							if(real_niches.size() == 1 )
							{
								cerr << "Warning:: In function Epoch:: Exist only one niche\n";
								break;
							}
						}

						random_father = real_niches[random_niche_father].organism_position[rand()%real_niches[random_niche_father].organism_position.size()];
						organism_father = prev_organisms[random_father];						

						random_mother = real_niches[random_niche_mother].organism_position[rand()%real_niches[random_niche_mother].organism_position.size()];
						organism_mother = prev_organisms[random_mother];
						
						organism_temp = crossover(organism_father, organism_mother);
					}
					else
					{
						random_niche_father = i;
						
						random_father = current_niches[random_niche_father].organism_position[rand()%current_niches[random_niche_father].organism_position.size()];
						organism_father = prev_organisms[random_father];
						
						while(true)
						{
							random_mother = rand() % current_niches[random_niche_father].organism_position.size();
							if(random_mother != random_father) break;
							if(current_niches[random_niche_father].organism_position.size() == 1) break;
						}
						
						organism_mother = prev_organisms[random_mother];
						random_mother = current_niches[random_mother].organism_position[random_mother];
						organism_temp = crossover(organism_father, organism_mother);
					}
				}

				if(rand()%100 < LARGE_POPULATION_DISCRIMINATOR)
				{ // enter if is a small niche
					if((rand()%1000)/1000.0 < SMALLER_POPULATIONS_PROBABILITY_ADDING_NEW_NODE)
					{
						organism_temp = mutation_node(organism_temp);
					}
					if((rand()%1000)/1000.0 < SMALLER_POPULATIONS_PROBABILITY_ADDING_NEW_CONNECTION) 
					{
						organism_temp = mutation_connection(organism_temp);
					}
				}
				else
				{// enter if is a large niche
					if((rand()%1000)/1000.0 < LARGER_POPULATIONS_PROBABILITY_ADDING_NEW_NODE)
					{
						organism_temp = mutation_node(organism_temp);
					}
					if((rand()%1000)/1000.0 < LARGER_POPULATIONS_PROBABILITY_ADDING_NEW_CONNECTION) 
					{
						organism_temp = mutation_connection(organism_temp);
					}
				}

				organisms.push_back(organism_temp);
			}
		}
	}

	spatiation();
}

#endif