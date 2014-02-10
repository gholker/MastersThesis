#ifndef BITWISE_WEIGHTS
#define BITWISE_WEIGHTS

#include <iostream>
#include "BitwiseWeights.hpp"
#include "Rand.hpp"
#include "Enumerations.h"

CfgFile* BitwiseWeights::config = NULL;

void BitwiseWeights::set_config( CfgFile& cfg )
{
    config = &cfg;
}


BitwiseWeights::BitwiseWeights()
{
    weights = NULL;
    weight_values = NULL;
    number_of_weights = 0;
}

BitwiseWeights::BitwiseWeights( int num )
{
    weight_values = NULL;
    weights = NULL;
    number_of_weights = 0;
    init( num );
}

BitwiseWeights::~BitwiseWeights()
{
    delete_weights();
    delete_weight_values();
}

void BitwiseWeights::delete_weights()
{
    if( weights != NULL )
    {
        delete [] weights;
        weights = NULL;
        number_of_weights = 0;
    }
}

void BitwiseWeights::delete_weight_values()
{
    if( weight_values )
    {
        delete [] weight_values;
        weight_values = NULL;
    }
}

void BitwiseWeights::init( int num )
{
    delete_weights();
    number_of_weights = num;
    weights = new BitwiseWeight[num];
}

bitwise_weight_value* BitwiseWeights::get_weights( RandomNumberGenerator& rng )
{
    delete_weight_values();
    weight_values = new bitwise_weight_value[number_of_weights];
    for( int i = 0; i < number_of_weights; i++ )
        weight_values[i] = weights[i].get_weight( rng );
    return weight_values;
}

void BitwiseWeights::pipe_increment( bitwise_weight_value* values )
{
    for( int i = 0; i < number_of_weights; i++ )
		if( values[i].is_used )
			weights[i].pipe_increment( values[i].integer );
}

void BitwiseWeights::normalize( bitwise_weight_value* values )
{
    for( int i = 0; i < number_of_weights; i++ )
		if( values[i].is_used )
	       weights[i].normalize( values[i].integer );
}

double BitwiseWeights::get_probability( bitwise_weight_value* values )
{
    double p = 1.;
    for( int i = 0; i < number_of_weights; i++ )
		if( values[i].is_used )
			p *= weights[i].get_probability( values[i].integer );
    return p;
}

void BitwiseWeights::pipe_adapt( bitwise_weight_value* values, int fitnessBest, int fitnessElite )
{
	double probabilityInput, probabilityTarget;
	double pinputold, newoldratio = 2., ratio_limit = 1.0;
	pinputold = probabilityInput  = get_probability( values );
	probabilityTarget = probabilityInput
					+ (1 - probabilityInput) * config->pipe_learning_rate
					* (config->pipe_fitness_constant + fitnessBest) /
                      (config->pipe_fitness_constant + fitnessElite);

	using namespace std;

	while( probabilityInput < probabilityTarget && newoldratio > ratio_limit )
	{
        pipe_increment( values );
		pinputold = probabilityInput;
		probabilityInput = get_probability( values );
		newoldratio = probabilityInput / pinputold;
	}
	normalize( values );
}

void BitwiseWeights::pipe_mutate( RandomNumberGenerator& rng )
{
    for( int i = 0; i < number_of_weights; i++ )
        weights[i].pipe_mutate( number_of_weights * 17, rng ); // 170 is the size; 17 bits * N weights
}


void BitwiseWeights::pbil_increment( bitwise_weight_value* values )
{
    for( int i = 0; i < number_of_weights; i++ )
		if( values[i].is_used )
			weights[i].pbil_increment( values[i].integer );
}

void BitwiseWeights::pbil_adapt( bitwise_weight_value* values )
{
	pbil_increment( values );
	normalize( values );
}

void BitwiseWeights::pbil_mutate( RandomNumberGenerator& rng )
{
	for( int i = 0; i < number_of_weights; i++ )
        weights[i].pbil_mutate( rng ); 
}


void BitwiseWeights::guided_mutation( bitwise_weight_value *elite_values, bitwise_weight_value *new_values, RandomNumberGenerator& rng )
{
	for( int i = 0; i < number_of_weights; i++ )
		new_values[i] = weights[i].guided_mutation( elite_values[i], new_values[i], rng );
}

void BitwiseWeights::print( std::ostream& out )
{
    for( int i = 0; i < number_of_weights; i++ )
    {
        weights[i].print( out );
        out << endline;
    }
}



#endif
