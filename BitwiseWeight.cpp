#ifndef BITWISE_WEIGHT
#define BITWISE_WEIGHT

#include "BitwiseWeight.hpp"
#include "Rand.hpp"
#include "Enumerations.h"
#include <iostream>

CfgFile* BitwiseWeight::config = NULL;

void BitwiseWeight::set_config( CfgFile&  cfg )
{
	config = &cfg;
}

BitwiseWeight::BitwiseWeight()
{
    elements = NULL;
    number_of_elements = 0;
    init( 17 );
}

BitwiseWeight::~BitwiseWeight()
{
    delete_elements();
}

void BitwiseWeight::delete_elements()
{
    if( elements != NULL )
        delete [] elements;
}

void BitwiseWeight::init( int num )
{
    delete_elements();
    number_of_elements = num;
    elements = new ProbabilisticModelElement[num];
    for( int i = 0; i < num; i++ )
        elements[i].init( 2 );
}

double BitwiseWeight::get_decimal_value( int int_value )
{
	int i;
	double value = 1.;
	if( int_value < 0 )
	{
		value *= -1;
		int_value *= -1;
	}
    int max_size = 2;
	for( i = 0; i < number_of_elements - 1; i++ )
        max_size *= 2;
	max_size--;
    value *= config->bw_range * int_value / max_size;
    return value;
}

bitwise_weight_value BitwiseWeight::get_weight( RandomNumberGenerator& rng )
{
    int i;
    bitwise_weight_value weight;
    weight.decimal = 0.;
    weight.integer = 0;
    int max_size = 2;
    int num = 0x1;
    for( i = 0; i < number_of_elements - 1; i++ )
    {
        if( elements[i].get_value( rng ) == 1 )
            weight.integer += num;
        num <<= 1;
        max_size *= 2;
    }
    max_size--;
    if( elements[i].get_value( rng ) == 1 )
        weight.integer *= -1;
	weight.decimal = get_decimal_value( weight.integer );
    return weight;
}

void BitwiseWeight::pipe_increment( int int_weight )
{
    elements[number_of_elements-1].pipe_increment( int_weight < 0 );
    if( int_weight < 0 ) int_weight *= -1;

    for( int i = 0; i < number_of_elements - 1; i++ )
    {
        elements[i].pipe_increment( int_weight % 2 );
        int_weight >>= 1;
    }
}

void BitwiseWeight::pipe_mutate( int size, RandomNumberGenerator& rng )
{
    for( int i = 0; i < number_of_elements; i++ )
        elements[i].pipe_mutate( size, rng );
}

void BitwiseWeight::pbil_increment( int int_weight )
{
	elements[number_of_elements-1].pbil_increment( int_weight < 0 );
    if( int_weight < 0 ) int_weight *= -1;

    for( int i = 0; i < number_of_elements - 1; i++ )
    {
        elements[i].pbil_increment( int_weight % 2 );
        int_weight >>= 1;
    }
}

void BitwiseWeight::pbil_mutate( RandomNumberGenerator& rng )
{
	for( int i = 0; i < number_of_elements; i++ )
        elements[i].pbil_mutate( rng );
}

void BitwiseWeight::normalize( int int_weight )
{
    elements[number_of_elements-1].normalize( int_weight < 0 );
    if( int_weight < 0 ) int_weight *= -1;

    for( int i = 0; i < number_of_elements - 1; i++ )
    {
        elements[i].normalize( int_weight % 2 );
        int_weight >>= 1;
    }
}

double BitwiseWeight::get_probability( int int_weight )
{
    double p = 1.;
    elements[number_of_elements-1].get_probability( int_weight < 0 );
    if( int_weight < 0 ) int_weight *= -1;

    for( int i = 0; i < number_of_elements - 1; i++ )
    {
        p *= elements[i].get_probability( int_weight % 2 );
        int_weight >>= 1;
    }
    return p;
}

bitwise_weight_value BitwiseWeight::guided_mutation( bitwise_weight_value elite_value, bitwise_weight_value new_value, RandomNumberGenerator& rng )
{
	int i;
	bitwise_weight_value weight;
	weight.integer = 0;
	int elite = elite_value.integer, newv = new_value.integer;
	bool is_neg;
	if( rng.getRandom() > config->gm_prob_of_mutation )
		is_neg = elite < 0;
	else 
		is_neg = newv < 0;
	if( elite < 0 ) elite *= -1;
	if( newv < 0 ) newv *= -1;
	int num = 0x1;

	for( i = 0; i < number_of_elements - 1; i++ )
	{
		bool gm = rng.getRandom() < config->gm_prob_of_mutation;
		if(		( gm && elite % 2 )
			||  (!gm && newv % 2 ) )
			weight.integer += num;
		num <<= 1;
		elite >>= 1;
		newv >>= 1;
	}
	if( is_neg )
		weight.integer *= -1;
	weight.decimal = get_decimal_value( weight.integer );
	return weight;
}

void BitwiseWeight::print( std::ostream& out )
{
    out.precision( 2 );
    for( int i = 0; i < 2; i++ )
    {
        for( int j = 0; j < number_of_elements; j++ )
            out << elements[j].get_probability( i ) << " ";
        out << endline;
    }
}

#endif
