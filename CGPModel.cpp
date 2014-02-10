#ifndef CGP_MODEL
#define CGP_MODEL

#include "CGPModel.hpp"
#include "DataCollector.hpp"
#include <iostream>

CfgFile* CGPModel::config = NULL;

void CGPModel::set_config( CfgFile& cfg )
{
    config = &cfg;
}

CGPModel::CGPModel()
{
	genotype = NULL;
    if( !config )
        DataCollector::debug << "CGPModel::config is NULL" << endline;
    else
        traverse_model( NULL, INIT_MODEL );
}

CGPModel::~CGPModel()
{
    delete [] node_elements;
    delete [] output_elements;
	delete_genotype();
}

void CGPModel::delete_genotype()
{
	if( genotype )
	{
		delete [] genotype;
		genotype = NULL;
	}
}

int CGPModel::length()
{
	return config->get_cgp_length();
}

void CGPModel::print( ostream& fout )
{
	traverse_model( &fout, PRINT );
}

// LEARNING

int* CGPModel::get_genotype( RandomNumberGenerator& rng )
{
	return (int*)traverse_model( (void*)&rng, GET_GENOTYPE );
}

double CGPModel::get_probability( int *gene )
{
	return *(double*)traverse_model( (void*)gene, GET_PROBABILITY );
}

void CGPModel::normalize( int *gene )
{
	traverse_model( (void*)gene, NORMALIZE );
}

void CGPModel::pipe_mutate( RandomNumberGenerator& rng )
{
	traverse_model( (void*)&rng, PIPE_MUTATE );
}

void CGPModel::pipe_increment( int *gene )
{
	traverse_model( (void*)gene, PIPE_INCREMENT );
}

void CGPModel::guided_mutation( int *elite_gene, int *new_gene, RandomNumberGenerator& rng )
{
	//rng.setSeed( seed );

	// GENE
	for( int i = 0; i < config->get_cgp_length(); i++ )
		if( rng.getRandom() < config->gm_prob_of_mutation )
			new_gene[i] = elite_gene[i];

}

void CGPModel::pipe_adapt( int *gene, int fitnessBest, int fitnessElite )
{
	double probabilityInput, probabilityTarget;
	double pinputold, newoldratio = 2., ratio_limit = 1.0;
	pinputold = probabilityInput  = get_probability( gene );
	probabilityTarget = probabilityInput
					+ (1 - probabilityInput) * config->pipe_learning_rate
					* (config->pipe_fitness_constant + fitnessBest) /
                      (config->pipe_fitness_constant + fitnessElite);

	using namespace std;
	int count = 0;
	while( probabilityInput < probabilityTarget && newoldratio > ratio_limit )
	{
		count++;
        pipe_increment( gene );
		pinputold = probabilityInput;
		probabilityInput = get_probability( gene );
		newoldratio = probabilityInput / pinputold;
		//if( count % 1000 == 0 || ( ( count != 1 ) && ( ( count-1 ) % 1000 == 0 ) ) )
		//	std::cout << "count=" << count << ", ratio=" << newoldratio << endline;
	}
	normalize( gene );
}


// TRAVERSAL FUNCTION

void* CGPModel::traverse_model( void *param, int directive )
{
	RandomNumberGenerator *rng = NULL;
	int *gene = NULL;
	bool *nodes_used = NULL;
	probability = 1.;
	int genotype_index = 0;
	ostream* fout = NULL;

	switch( directive )
	{
    case INIT_MODEL:
    	number_of_nodes = config->cgp_number_of_rows * config->cgp_number_of_columns;
        node_elements = new CGPModelNode[number_of_nodes];
        output_elements = new ProbabilisticModelElement[config->number_of_outputs];
        break;
	case PRINT:
		fout = (ostream*)param;
		break;
	case GET_GENOTYPE:
		delete_genotype();
		genotype = new int[ this->length() ];
		// INTENTIONAL: there is no break;
	case PIPE_MUTATE:
	case PBIL_MUTATE:
		rng = (RandomNumberGenerator*)param;
		break;
	case PIPE_INCREMENT:
	case PBIL_INCREMENT:
	case GET_PROBABILITY:
	case NORMALIZE:
		gene = (int*)param;
		nodes_used = new bool[number_of_nodes];
		for( int i = 0; i < number_of_nodes; i++ )
			nodes_used[i] = false;
		CGPGraph graph;
		double *weights = new double[config->get_cgp_num_weights()];
		graph.init( gene, weights );
		delete [] weights;
		graph.find_used_nodes( nodes_used, NULL );
		break;
	}

	for( int column = 0; column < config->cgp_number_of_columns; column++ )
		for( int row = 0; row < config->cgp_number_of_rows; row++ )
		{
			int element_index = config->cgp_number_of_rows * column + row;
			int gene_index = element_index * CGPModelNode::length();
			switch( directive )
			{
            case INIT_MODEL:
                {
                    int number_of_possibilities = config->get_cgp_number_of_possibilities( column );
                    node_elements[ element_index ].init( config->number_of_inputs + number_of_possibilities );
                }
                break;
			case PRINT:
				node_elements[element_index].print( *fout );
				break;
			case GET_GENOTYPE:
				{
					int* node_gene = node_elements[element_index].get_genotype( *rng );
					genotype_index = gene_index;
					for( int i = 0; i < CGPModelNode::length(); i++ )
					{
						genotype[genotype_index] = node_gene[i];
						if( genotype[genotype_index] >= config->number_of_inputs )
						{
							int x = column - config->cgp_levels_back;
							genotype[genotype_index] += ( x > 0 ? x : 0 ) * config->cgp_number_of_rows;
						}
						genotype_index++;
					}
				}
				break;
			case GET_PROBABILITY:
				if( nodes_used[element_index] )
					probability *= node_elements[element_index].get_probability( &gene[gene_index], column );
				break;
			case NORMALIZE:
				node_elements[element_index].normalize( &gene[gene_index], column );
				break;
			case PIPE_MUTATE:				
				node_elements[element_index].pipe_mutate( this->length(), *rng );
				break;
			case PIPE_INCREMENT:
				if( nodes_used[element_index] )
					node_elements[element_index].pipe_increment( &gene[gene_index], column );
				break;
			case PBIL_MUTATE:				
				node_elements[element_index].pbil_mutate( *rng );
				break;
			case PBIL_INCREMENT:
				if( nodes_used[element_index] )
					node_elements[element_index].pbil_increment( &gene[gene_index], column );
				break;
			}
		}
	for( int output = 0; output < config->number_of_outputs; output++ )
	{
		int gene_index = config->get_cgp_length() - config->number_of_outputs + output;
		int gene_value = 0;
		switch( directive )
		{
        case GET_PROBABILITY:
        case PIPE_INCREMENT:
		case PBIL_INCREMENT:
        case NORMALIZE:
            gene_value = gene[gene_index];
            if( gene_value >= config->number_of_inputs )
                gene_value -= config->get_cgp_number_of_inaccessible_nodes( config->cgp_number_of_columns );
            break;
		}
		switch( directive )
		{
        case INIT_MODEL:
            output_elements[output].init( config->number_of_inputs
                                        + config->cgp_levels_back * config->cgp_number_of_rows );
            break;
		case PRINT:
			output_elements[output].print( *fout );
			break;
		case GET_GENOTYPE:
			{
				genotype[genotype_index] = output_elements[output].get_value( *rng );
				int x = config->cgp_number_of_columns - config->cgp_levels_back;
				genotype[genotype_index] += ( x > 0 ? x : 0 ) * config->cgp_number_of_rows;
				genotype_index++;
			}
			break;
		case GET_PROBABILITY:
			probability *= output_elements[output].get_probability( gene_value );
			break;
		case NORMALIZE:
			output_elements[output].normalize( gene_value );
			break;
		case PIPE_MUTATE:
			output_elements[output].pipe_mutate( this->length(), *rng );
			break;
		case PIPE_INCREMENT:
			output_elements[output].pipe_increment( gene_value );
			break;
		case PBIL_MUTATE:
			output_elements[output].pbil_mutate( *rng );
			break;
		case PBIL_INCREMENT:
			output_elements[output].pbil_increment( gene_value );
			break;
		}
	}

	if( nodes_used )
		delete [] nodes_used;

	switch( directive )
	{
	case GET_GENOTYPE:
		return (void*)genotype;
	case GET_PROBABILITY:
		return (void*)&probability;
	default:
		return NULL;
	}
}

void CGPModel::pbil_mutate( RandomNumberGenerator& rng )
{
	traverse_model( (void*)&rng, PBIL_MUTATE );
}

void CGPModel::pbil_increment( int *gene )
{
	traverse_model( (void*)gene, PBIL_INCREMENT );
}

void CGPModel::pbil_adapt( int *gene )
{
	pbil_increment( gene );
	normalize( gene );
}

#endif
