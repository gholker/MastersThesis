
#ifndef INDIVIDUAL
#define INDIVIDUAL

#include "Individual.hpp"
#include "DataCollector.hpp"

CfgFile* Individual::config = NULL;

void Individual::set_config( CfgFile& cfg )
{
    config = &cfg;
}

Individual::Individual()
{
    if( config == NULL )
        DataCollector::debug << "Individual::config is NULL." << endline;
    fitness = 0;
    gep_genes = NULL;
    gepnet = NULL;
    weight_values = NULL;
	weights = NULL;
	cgp_weights_used = NULL;
	if( config->representation == GEP )
		init_gep_genes();
    //if( config->weight_representation == BITWISE )
    {
        weight_values = new bitwise_weight_value*[config->number_of_weight_arrays];
        for( int i = 0; i < config->number_of_weight_arrays; i++ )
            weight_values[i] = new bitwise_weight_value[config->number_of_weights];
    }
	//else if( config->weight_representation == STOCHASTIC_HILL_CLIMBING )
	{
		weights = new double*[config->number_of_weight_arrays];
        for( int i = 0; i < config->number_of_weight_arrays; i++ )
            weights[i] = new double[config->number_of_weights];
	}
}

Individual::~Individual()
{
    delete_gep_genes();
    delete_gepnet();
    delete_weight_values();
	delete_weights();
	delete_weights_used();
}

void Individual::delete_weight_values()
{
    if( weight_values )
	{
		for( int i = 0; i < config->number_of_weight_arrays; i++ )
			if( weight_values[i] )
				delete [] weight_values[i];
		delete [] weight_values;
	}
}

void Individual::delete_weights_used()
{
	if( cgp_weights_used )
	{
		delete [] cgp_weights_used;
		cgp_weights_used = NULL;
	}
}

// SHC Weights

void Individual::set_weights( int index, double *weights )
{
	for( int w = 0; w < config->number_of_weights; w++ )
		this->weights[index][w] = weights[w];
}

double* Individual::get_weights( int index )
{
	return weights[index];
}

void Individual::delete_weights()
{
    if( weights )
	{
		for( int i = 0; i < config->number_of_weight_arrays; i++ )
			if( weights[i] )
				delete [] weights[i];
		delete [] weights;
	}
}

// weights used

bool* Individual::get_weights_used( int index )
{
	if( config->representation == GEP )
		return this->get_gep_genes()[index]->getWeightsUsed();
	else if( config->representation == CGP )
	{
		if( !cgp_weights_used )
		{
			delete_weights_used();
			cgp_weights_used = new bool[config->get_cgp_num_weights()];
			for( int i = 0; i < config->get_cgp_num_weights(); i++ )
				cgp_weights_used[i] = false;
			cgp_graph.find_used_nodes( NULL, cgp_weights_used );
		}
		return cgp_weights_used;
	}
	else
		return NULL;
}

// EVALUATE

const double* Individual::evaluate( double *inputs )
{
    switch( config->representation )
    {
	case GEP:
        return gepnet->evaluate( inputs );
	case CGP:
        return cgp_graph.evaluate( inputs );
	default:
		DataCollector::debug << "Model is not in list (individual::evaluate)" << endline;
        return NULL;
    }
}

// PRINT

void Individual::print( ostream& fout )
{
    fout << "fitness=" << fitness << ":" << endline;
    switch( config->representation )
    {
	case GEP:
        for( int i = 0; i < config->number_of_weight_arrays; i++ )
            gep_genes[i]->print( fout );
		break;
	case CGP:
		cgp_graph.print( fout );
		break;
    }
}

// BITWISE

void Individual::set_weight_values( int index, bitwise_weight_value *values )
{
    for( int i = 0; i < config->number_of_weights; i++ )
        weight_values[index][i] = values[i];
}

bitwise_weight_value* Individual::get_weight_values( int index )
{
	// INSERT WHETHER THE WEIGHT WAS USED INTO THE WEIGHT VALUES
	bool *used = get_weights_used( index );
	for( int i = 0; i < config->number_of_weights; i++ )
		weight_values[index][i].is_used = used[i];
    return weight_values[index];
}

// GEP

void Individual::init_gep_genes()
{
    if( !gep_genes )
        gep_genes = new GepNN*[config->number_of_outputs];
}

void Individual::delete_gep_genes()
{
    if( gep_genes )
    {
        for( int i = 0; i < config->number_of_outputs; i++ )
            if( gep_genes[i] != NULL )
                delete gep_genes[i];
        delete [] gep_genes;
        gep_genes = NULL;
    }
}

void Individual::set_gep_gene( GepNN *gene, int i )
{
    gep_genes[i] = gene;
}

GepNN** Individual::get_gep_genes()
{
    return gep_genes;
}

void Individual::delete_gepnet()
{
    if( gepnet )
        delete gepnet;
}

void Individual::set_gepnet( MIMO_FF_GEPNet *net )
{
    gepnet = net;
}

MIMO_FF_GEPNet* Individual::get_gepnet()
{
    return gepnet;
}

// CGP

void Individual::cgp_graph_init( int *gene, double *weights )
{
	cgp_graph.init( gene, weights );
}

int* Individual::get_cgp_array()
{
	return cgp_graph.get_gene();
}

#endif
