#ifndef CGP_MODEL_NODE
#define CGP_MODEL_NODE

#include "CGPModelNode.hpp"
#include "DataCollector.hpp"
#include <iostream>

CfgFile* CGPModelNode::config = NULL;

void CGPModelNode::set_config( CfgFile& cfg )
{
	config = &cfg;
}

int CGPModelNode::length()
{
	if( config )
		return ( config->cgp_weight_redirect == YES ? 3 : 2 ) * config->cgp_arity + 1;
	else
	{
		DataCollector::debug << "WARNING: config not defined for CGPModelNode." << endline;
		return -1;
	}
}

CGPModelNode::CGPModelNode()
{
	if( !config )
		DataCollector::debug << "CGModelNode config is NULL" << endline;
	gene = NULL;
	elements = NULL;
}

CGPModelNode::~CGPModelNode()
{
	if( elements )
		delete [] elements;
    delete_gene();
}

void CGPModelNode::init( int number_of_connections )
{
	int i;
	number_of_elements = CGPModelNode::length();
	int modifier = ( config->cgp_weight_redirect == YES ? 3 : 2 );
	elements = new ProbabilisticModelElement[number_of_elements];
	for( i = 0; i < modifier * config->cgp_arity; i += modifier )
	{
		elements[i].init( number_of_connections );
		elements[i+1].init( 2 ); // CONNECTION BIT on odd indices
		if( config->cgp_weight_redirect == YES )
			elements[i+2].init( config->cgp_num_weights );
	}
	elements[i].init( config->cgp_number_of_functions );
}

void CGPModelNode::delete_gene()
{
	if( gene )
	{
		delete [] gene;
		gene = NULL;
	}
}

void CGPModelNode::print( ostream& fout )
{
	fout.width( 7 );
	for( int i = 0; i < CGPModelNode::length(); i++ )
		elements[i].print( fout );
}

int CGPModelNode::get_value( int gene_value, int column )
{
    if( gene_value >= config->number_of_inputs )
        gene_value -= config->get_cgp_number_of_inaccessible_nodes( column );
    return gene_value;
}

// LEARNING METHODS

int* CGPModelNode::get_genotype( RandomNumberGenerator& rng )
{
	delete_gene();
	gene = new int[number_of_elements];
	for( int i = 0; i < number_of_elements; i++ )
	{
		gene[i] = elements[i].get_value( rng );
	}
	return gene;
}

double CGPModelNode::get_probability( int *gene, int column )
{
	double p = 1.;
	int i;
	for( i = 0; i < number_of_elements - 1; i += 3 )
	{
		p *= elements[i+1].get_probability( get_value( gene[i+1], column ) );	
		if( get_value( gene[i+1], column ) )
		{
			p *= elements[i].get_probability( get_value( gene[i], column ) );	
			p *= elements[i+2].get_probability( get_value( gene[i+2], column ) );	
		}
	}
	p *= elements[i].get_probability( get_value( gene[i], column ) );
	return p;
}

void CGPModelNode::normalize( int *gene, int column )
{
	for( int i = 0; i < number_of_elements; i++ )
		elements[i].normalize( get_value( gene[i], column ) );
}

void CGPModelNode::pipe_mutate( int size, RandomNumberGenerator& rng )
{
	for( int i = 0; i < number_of_elements; i++ )
		elements[i].pipe_mutate( size, rng );
}

void CGPModelNode::pipe_increment( int *gene, int column )
{
	int i;
	for( i = 0; i < number_of_elements - 1; i += 3 )
	{
		elements[i+1].pipe_increment( get_value( gene[i+1], column ) );	
		if( get_value( gene[i+1], column ) )
		{
			elements[i].pipe_increment( get_value( gene[i], column ) );
			elements[i+2].pipe_increment( get_value( gene[i+2], column ) );
		}
	}
	elements[i].pipe_increment( get_value( gene[i], column ) );
}

void CGPModelNode::pbil_mutate( RandomNumberGenerator& rng )
{
	for( int i = 0; i < number_of_elements; i++ )
		elements[i].pbil_mutate( rng );
}

void CGPModelNode::pbil_increment( int *gene, int column )
{
	int i;
	for( i = 0; i < number_of_elements - 1; i += 3 )
	{
		elements[i+1].pbil_increment( get_value( gene[i+1], column ) );	
		if( get_value( gene[i+1], column ) )
		{
			elements[i].pbil_increment( get_value( gene[i], column ) );
			elements[i+2].pbil_increment( get_value( gene[i+2], column ) );
		}
	}
	elements[i].pbil_increment( get_value( gene[i], column ) );
}

#endif
