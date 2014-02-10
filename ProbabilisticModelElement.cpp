#ifndef PROB_MODEL_ELEM
#define PROB_MODEL_ELEM

#include "ProbabilisticModelElement.hpp"
#include "DataCollector.hpp"
#include "Rand.hpp"
#include <cmath>
#include <iostream>

ProbabilisticModelElement::ProbabilisticModelElement()
{
    probabilities = NULL;
    number_of_probabilities = 0;
}

ProbabilisticModelElement::ProbabilisticModelElement( int num )
{
    init( num );
}

ProbabilisticModelElement::~ProbabilisticModelElement()
{
    delete_probabilities();
}

int ProbabilisticModelElement::get_length()
{
	return number_of_probabilities;
}

void ProbabilisticModelElement::init( int num )
{
    double val = 1.0 / num;
    delete_probabilities();
    number_of_probabilities = num;
    probabilities = new double[num];
    for( int i = 0; i < num; i++ )
        probabilities[i] = val;
}

void ProbabilisticModelElement::delete_probabilities()
{
    if( probabilities )
    {
        delete [] probabilities;
        probabilities = NULL;
    }
}

void ProbabilisticModelElement::print( ostream& fout )
{
	fout.width( 7 );
	fout.precision( 4 );
	for( int i = 0; i < number_of_probabilities; i++ )
		fout << probabilities[i] << " ";
	fout << endline;
}

// LEARNING

int ProbabilisticModelElement::get_value( RandomNumberGenerator& rng )
{
    double sum = 0., p = rng.getRandom();
    for( int i = 0; i < number_of_probabilities; i++ )
    {
        sum += probabilities[i];
        if( sum >= p )
            return i;
    }
	if( number_of_probabilities != 1 )
	{
		DataCollector::debug	<< "Warning, returning last value from PME::get_value.\nsum(probs)=" 
								<< sum << ", p=" << p << " expname=" << config->experiment_name << endline;		
		normalize_fix();
	}
    return number_of_probabilities - 1;
}

void ProbabilisticModelElement::normalize_fix()
{
	double sum = 0.;
	for( int i = 0; i < number_of_probabilities; i++)
		sum += probabilities[i];
	for( int i = 0; i < number_of_probabilities; i++ )
		probabilities[i] /= sum;
}

// Taken from PIPE
void ProbabilisticModelElement::normalize( int index )
{
	double sum = 0., old_probability, gamma;

	if( number_of_probabilities == 1 )
	{
		probabilities[0] = 1.;
		return;
	}
	else if( number_of_probabilities == 2 )
	{
		probabilities[( index + 1 ) % 2] = 1.0 - probabilities[index];
		return;
	}

	if( index >= number_of_probabilities || index < 0 )
		DataCollector::debug << endline << "index=" << index << ", #prob=" << number_of_probabilities << endline;

    for( int i = 0; i < number_of_probabilities; i++ )
        sum += probabilities[i];

    old_probability = probabilities[index] - (sum - 1.0);

    if( old_probability == 1.0 )
        gamma = 0.0;
    else
        gamma = ( probabilities[index] - old_probability )
                / (1.0 - old_probability);

    for( int i = 0; i < number_of_probabilities; i++ )
        if( i != index )
            probabilities[i] *= (1.0 - gamma);
}

void ProbabilisticModelElement::pipe_mutate( int size, RandomNumberGenerator& rng )
{
    double prob_mutate =    config->pipe_prob_mutation
                        / ( number_of_probabilities * sqrt( (double)size ) );
    for( int i = 0; i < number_of_probabilities; i++ )
        if( rng.getRandom() < prob_mutate )
		{
            probabilities[i] += config->pipe_mutation_rate * ( 1 - probabilities[i] );
			this->normalize( i );
		}
}

double ProbabilisticModelElement::get_probability( int index )
{
    if( index < number_of_probabilities && index >= 0 )
        return probabilities[index];
    else
        return -1.;
}

void ProbabilisticModelElement::pipe_increment( int index )
{
    //std::cout << "index = " << index << "lr=" << config->pipe_learning_rate << std::endl;
    probabilities[index] += 0.1 * config->pipe_learning_rate * ( 1 - probabilities[index] );
}

CfgFile* ProbabilisticModelElement::config = NULL;

void ProbabilisticModelElement::set_config( CfgFile& cfg )
{
    config = &cfg;
}

void ProbabilisticModelElement::pbil_increment( int index )
{
	probabilities[index] *= ( 1. - config->pbil_learning_rate );
	probabilities[index] += config->pbil_learning_rate;
}

void ProbabilisticModelElement::pbil_mutate( RandomNumberGenerator& rng )
{
	for( int i = 0; i < number_of_probabilities; i++ )
		if( rng.getRandom() < config->pbil_mutation_probability )
		{
			probabilities[i] *= ( 1. - config->pbil_mutation_shift );
			probabilities[i] += ( rng.getRandom() > .5 ) * config->pbil_mutation_shift;
			this->normalize( i );
		}
}

#endif
