
#ifndef PPC
#define PPC

#include <iostream>
#include <fstream>
#include "PPC.hpp"
#include "SHCWeight.hpp"
#include "GEPNN.hpp"
#include <cmath>

CfgFile* PrototypeChromosome::config = NULL;

void PrototypeChromosome::set_config( CfgFile& cfg )
{
	config = &cfg;
}

PrototypeChromosome::PrototypeChromosome()
{

}

void PrototypeChromosome::init()
{

	num_functions		= config->gep_max_arity - config->gep_min_arity + 1;
	tail_length		    = ( ( config->gep_head_length * ( config->gep_max_arity - 1 ) ) + 1);
	weight_domain_length= ( config->gep_head_length * config->gep_max_arity );
	gene_length			= config->gep_head_length + tail_length + weight_domain_length;
	head_tail_length	= config->gep_head_length + tail_length;
	elements = new ProbabilisticModelElement*[config->gep_num_genes];

	for( int i = 0; i < config->gep_num_genes; i++ )
	{
		elements[i] = new ProbabilisticModelElement[gene_length];
		for( int j = 0; j < gene_length; j++ )
		{
			if( j < config->gep_head_length )
				elements[i][j].init( num_functions + config->number_of_inputs );
			else if( j < config->gep_head_length + tail_length )
				elements[i][j].init( config->number_of_inputs );
			else
				elements[i][j].init( config->gep_num_weights );
		}
	}

}


PrototypeChromosome::~PrototypeChromosome()
{
	for( int i = 0; i < config->gep_num_genes; i++ )
		delete [] elements[i];
	delete [] elements;
}


GepNN* PrototypeChromosome::get_genotype( RandomNumberGenerator& rng )
{
	short i, j;
	GepNN* gep;

	gep = new GepNN(    config->gep_num_genes, config->gep_head_length,
                        config->gep_min_arity, config->gep_max_arity,
                        config->number_of_inputs, config->gep_num_weights );

	short **gene = new short*[config->gep_num_genes];

	for( i = 0; i < config->gep_num_genes; i++ )
	{
		gene[i] = new short[gene_length];
		for( j = 0; j < gene_length; j++ )
			gene[i][j] = elements[i][j].get_value( rng );

		gep->setGene( i, gene[i] );
	}
	for( int i = 0; i < config->gep_num_genes; i++ )
		delete [] gene[i];
	delete [] gene;
	return gep;
}

void PrototypeChromosome::guided_mutation( GepNN *gep, GepNN *new_gep, RandomNumberGenerator& rng )
{
	for( int i = 0; i < config->gep_num_genes; i++ )
		for( int j = 0; j < gene_length; j++ )
			if( rng.getRandom() < config->gm_prob_of_mutation )
				new_gep->setGeneElem( i, j, gep->getGene( i )[j] );
}


void PrototypeChromosome::pipe_adapt( GepNN *gep, int fitness_best, int fitness_elite )
{
	double probabilityInput, probabilityTarget, oldProbabilityInput, newOldRatio = 2.0, ratio_limit = 1.0;
	int count = 0;

	oldProbabilityInput = probabilityInput  = get_probability( gep );
	probabilityTarget = probabilityInput
					+ (1 - probabilityInput) * config->pipe_learning_rate
					* (config->pipe_fitness_constant + fitness_best) / (config->pipe_fitness_constant + fitness_elite);

	while( probabilityInput < probabilityTarget && newOldRatio > ratio_limit )
	{
		count++;
		pipe_increment( gep );

		oldProbabilityInput = probabilityInput;
		probabilityInput = get_probability( gep );
		newOldRatio = probabilityInput / oldProbabilityInput;
	}
	normalize( gep );
}

void PrototypeChromosome::normalize( GepNN *gep )
{
	int i, j, os;
	short *gene;
	for( i = 0; i < config->gep_num_genes; i++ )
	{
		gene = gep->getGene( i );
		os = gep->getOffset( i );
		for( j = 0; j < gene_length; j++ )
			if( in_use( os, j ) )
				elements[i][j].normalize( gene[j] );
	}
}

bool PrototypeChromosome::in_use( int offset, int position )
{
	return		(  position < head_tail_length		&&	 position <= offset	  )
			|| ( ( position >= head_tail_length )	&& ( position < offset + head_tail_length ) );
}

double PrototypeChromosome::get_probability( GepNN *gep )
{
	int i, j;
	double p = 1.;
	int os;
	short *gene;
	for( i = 0; i < config->gep_num_genes; i++ )
	{
		os = gep->getOffset( i );
		gene = gep->getGene( i );
		for( j = 0; j < gene_length; j++ )
			if( in_use( os, j ) )
				p *= elements[i][j].get_probability( gene[j] );
	}
	return p;
}


void PrototypeChromosome::pipe_mutate( GepNN *gep, RandomNumberGenerator& rng )
{
	int i, j;
	int size = gep->getSize();
	int os;

	for( i = 0; i < config->gep_num_genes; i++ )
	{
		os = gep->getOffset( i );
		for( j = 0; j < gene_length; j++ )
			if( in_use( os, j ) )
				elements[i][j].pipe_mutate( size, rng );
	}
}

void PrototypeChromosome::pipe_increment( GepNN *gep )
{
	int i, j;
	int os;
	short *gene;
	for( i = 0; i < config->gep_num_genes; i++ )
	{
		os = gep->getOffset( i );
		gene = gep->getGene(i);
		for( j = 0; j < gene_length; j++ )
			if( in_use( os, j ) )
				elements[i][j].pipe_increment( gene[j] );
	}
}

void PrototypeChromosome::pbil_mutate( GepNN *gep, RandomNumberGenerator& rng )
{
	int i, j;
	int os;

	for( i = 0; i < config->gep_num_genes; i++ )
	{
		os = gep->getOffset( i );
		for( j = 0; j < gene_length; j++ )
			if( in_use( os, j ) )
				elements[i][j].pbil_mutate( rng );
	}
}

void PrototypeChromosome::pbil_increment( GepNN *gep )
{
	int i, j;
	int os;
	short *gene;
	for( i = 0; i < config->gep_num_genes; i++ )
	{
		os = gep->getOffset( i );
		gene = gep->getGene(i);
		for( j = 0; j < gene_length; j++ )
			if( in_use( os, j ) )
				elements[i][j].pbil_increment( gene[j] );
	}
}

void PrototypeChromosome::pbil_adapt( GepNN *gep )
{
	pbil_increment( gep );
	normalize( gep );
}

void PrototypeChromosome::print( std::ostream &fout )
{
	using namespace std;
	fout.width(7);
	for( int i = 0; i < config->gep_num_genes; i++ )
		for( int j = 0; j < gene_length; j++ )
			elements[i][j].print( fout );
}

#endif
