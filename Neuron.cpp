/* Neural Network Code for PDNE
 * Written by Graham Holker on 6-14-2010
 */


#ifndef NEURON
#define NEURON

#include <iostream>
#include <cmath>
#include "Neuron.hpp"
#include "DataCollector.hpp"

CfgFile *Neuron::config = NULL;

void Neuron::set_config( CfgFile& cfg )
{
    config = &cfg;
}

Neuron::Neuron( int num, double *in_weights, int num_weights )
{
    if( config == NULL )
        DataCollector::debug << "Neuron::config is NULL." << endline;
    init( num );
    setWeights( in_weights, num_weights );
}

Neuron::Neuron( int num )
{
    init( num );
}

Neuron::Neuron()
{
    // default to INPUT
    init( 0 );
}

void Neuron::init( int num )
{
    isOrNeuron = false;
    threshold = 1.0;
    this->num_inputs = (short)num;
    setValue( 0. );

    inputs = NULL;
   	initInputs();
	initWeights();
}

void Neuron::initInputs()
{
    if( !isInput() )
    {
        inputs = new Neuron*[num_inputs];
        for( int i = 0; i < num_inputs; i++ )
            inputs[i] = NULL;
    }
}

Neuron::~Neuron()
{
    // DELETE WEIGHTS
    if( !isInput() )
    {
        delete [] weights;
        weights = NULL;
    }
    // DELETE BRANCHES
	if( !isInput() )
	{
		for( int i = 0; i < num_inputs; i++ )
			if( inputs[i] && !inputs[i]->isInput() )
				delete inputs[i];
        delete [] inputs;
        inputs = NULL;
	}
}

bool Neuron::isInput()
{
	return num_inputs <= 0;
}

void Neuron::initWeights()
{
    if( !isInput() )
    {
        weights = new double[num_inputs];
        for( int i = 0; i < num_inputs; i++ )
            weights[i] = 0.;
    }
}

void Neuron::setWeights( double *w, int nw )
{
	for( int i = 0; ( i < num_inputs ) && ( i < nw ); i++ )
		weights[i] = w[i];
}

void Neuron::setWeight( short i, double weight )
{
	if( i < num_inputs )
		weights[i] = weight;
}

double Neuron::getWeight( short i )
{
	return weights[i];
}

void Neuron::setValue( double val )
{
	value = val;
}

double Neuron::getValue()
{
	return value;
}


// EVALUATE -- RECURSIVE
double Neuron::evaluate()
{
	// IF INPUT, RETURN VALUE
	if( isInput() )
        return value;

	// ELSE EVALUATE

	// FIND WEIGHTED SUM
	double sum = 0.;
	for( int i = 0; i < num_inputs; i++ )
		sum += weights[i] * inputs[i]->evaluate();

	// ACTIVATION FUNCTION
    switch( config->activation_function )
    {
    case SIGMOID:
		return fsigmoid( sum, config->activation_slope );
    case BINARY:
        return binary_activation( sum, threshold );
	case HYPERBOLIC_TANGENT:
		return hyperbolic_tangent( sum, config->activation_slope );
    }
    return 0.;
}

void Neuron::setLink( short i, Neuron *n )
{
	if( i < num_inputs )
	{
		inputs[i] = n;
	}
}



short Neuron::getNumInputs()
{
	return num_inputs;
}


#endif

