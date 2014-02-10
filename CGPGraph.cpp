#ifndef CGP_GRAPH
#define CGP_GRAPH

#include "CGPGraph.hpp"
#include "DataCollector.hpp"

CfgFile* CGPGraph::config = NULL;

void CGPGraph::set_config( CfgFile& cfg )
{
	config = &cfg;
}

CGPGraph::CGPGraph()
{
	gene = NULL;
	weights = NULL;
	inputs = NULL;
	outputs = NULL;
}

CGPGraph::~CGPGraph()
{
	if( gene )
	{
		delete [] gene;
		gene = NULL;
	}
	if( weights )
	{
		delete [] weights;
		weights = NULL;
	}
	delete_inputs();
	delete_outputs();
}

int* CGPGraph::get_gene()
{
	return gene;
}

double* CGPGraph::get_weight()
{
	return weights;
}

void CGPGraph::set_inputs( double *inputs )
{
	delete_inputs();
	this->inputs = new double[config->number_of_inputs];
	for( int i = 0; i < config->number_of_inputs; i++ )
		this->inputs[i] = inputs[i];
}

void CGPGraph::delete_inputs()
{
	if( inputs )
	{
		delete [] inputs;
		inputs = NULL;
	}
}

void CGPGraph::delete_outputs()
{
	if( outputs )
	{
		delete [] outputs;
		outputs = NULL;
	}
}

void CGPGraph::print( ostream& fout )
{
	for( int i = 0; i < config->get_cgp_length(); i++ )
		fout << this->gene[i] << " ";
	fout << endline;

	fout.precision( 2 );
	for( int i = 0; i < config->get_cgp_num_weights(); i++ )
		fout << this->weights[i] << " ";
	fout << endline;
}

void CGPGraph::init( int *gene, double *weights )
{
	this->gene = new int[config->get_cgp_length()];
	for( int i = 0; i < config->get_cgp_length(); i++ )
		this->gene[i] = gene[i];

	this->weights = new double[config->get_cgp_num_weights()];
	for( int i = 0; i < config->get_cgp_num_weights(); i++ )
		this->weights[i] = weights[i];
}

double* CGPGraph::evaluate( double *inputs )
{	
	set_inputs( inputs );
	delete_outputs();
	outputs = new double[config->number_of_outputs];
	int index_of_outputs = config->get_cgp_length() - config->number_of_outputs;
	for( int i = 0; i < config->number_of_outputs; i++ )
		outputs[i] = evaluate_node( gene[index_of_outputs + i] );
	return outputs;
}

double CGPGraph::evaluate_node( int index )
{
	if( index < 0 || index >= config->cgp_number_of_rows * config->cgp_number_of_columns + config->number_of_inputs )
		DataCollector::debug << endline << "index=" << index << ", #=" << config->cgp_number_of_rows * config->cgp_number_of_columns << endline;

	if( index < config->number_of_inputs )
		return inputs[index];
	else
	{
		double sum = 0;
		int i;
		int mod_index = index - config->number_of_inputs;
		int modifier = ( config->cgp_weight_redirect == YES ? 3 : 2 );

		mod_index *= ( modifier * config->cgp_arity + 1 );
		for( i = 0; i < modifier * config->cgp_arity - 1; i += modifier )
		{
			int connection_bit = gene[mod_index + i + 1];
			if( connection_bit )
			{
				int weight_index;
				if( config->cgp_weight_redirect == YES )
					weight_index = gene[mod_index + i + 2];
				else
					weight_index = config->get_cgp_weight_index( gene[mod_index + i], index );
				double weight = weights[ weight_index ];

				double node_value = evaluate_node( gene[mod_index + i] );
				sum += weight * node_value;
			}
		}
		switch( config->cgp_functions[ gene[i] ] )
		{
		case SIGMOID:
			sum = fsigmoid( sum, config->activation_slope );
			break;
		case BINARY:
			sum = binary_activation( sum, 1.0 );
			break;
		case HYPERBOLIC_TANGENT:
			sum = hyperbolic_tangent( sum, config->activation_slope );
			break;
		}
		return sum; 
	}
}

void CGPGraph::find_used_nodes( bool *nodes_used, bool *weights_used )
{
	int index_of_outputs = config->get_cgp_length() - config->number_of_outputs;
	for( int i = 0; i < config->number_of_outputs; i++ )
		find_used( gene[index_of_outputs + i], nodes_used, weights_used );
}

void CGPGraph::find_used( int index, bool *nodes_used, bool *weights_used )
{
	if( index > config->number_of_inputs )
	{
		int node_index = index - config->number_of_inputs;
		int i;
		int modifier = ( config->cgp_weight_redirect == YES ? 3 : 2 );
		int mod_index = index - config->number_of_inputs;
		mod_index *= ( modifier * config->cgp_arity + 1 );
		for( i = 0; i < modifier * config->cgp_arity; i += modifier )
		{
			int connection_bit = gene[mod_index + i + 1];
			if( connection_bit )
			{
				if( nodes_used )
					nodes_used[node_index] = true;

				if( weights_used )
				{
					int weight_index;
					if( config->cgp_weight_redirect == YES )
						weight_index = gene[mod_index + i + 2];
					else
						weight_index = config->get_cgp_weight_index( gene[mod_index + i], index );
					weights_used[weight_index] = true;
				}

				find_used( gene[mod_index + i], nodes_used, weights_used );
			}
		}
	}
}

#endif
