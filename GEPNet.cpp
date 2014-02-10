#ifndef NEURALNET
#define NEURALNET

#include <iostream>
#include <cmath>
#include "GEPNet.hpp"


GEPNet::GEPNet( Neuron *rt, Neuron **inps, int num_inputs )
{
	inputs = NULL;
	setInputs( inps, num_inputs );
	root = rt;
	should_delete = true;
}

GEPNet::~GEPNet()
{
	if( should_delete )
	{
		if( !root->isInput() )
			delete root;
	}
	if( inputs != NULL )
	{		
		delete [] inputs;
	}
}

double GEPNet::evaluate()
{
	return root->evaluate();
}

double GEPNet::evaluate( double *ivalues )
{
	for( int i = 0; i < num_inputs; i++ )
		inputs[i]->setValue( ivalues[i] );

	return evaluate();
}


void GEPNet::setInputs( Neuron **inps, int num_inputs )
{
	this->num_inputs = num_inputs;

	inputs = new Neuron*[num_inputs];
	for( int i = 0; i < num_inputs; i++ )
		inputs[i] = inps[i];
}

Neuron* GEPNet::getRoot()
{
	return root;
}


// Multiple Input, Multiple Output Feed-Forward Neural Network

MIMO_FF_GEPNet::MIMO_FF_GEPNet( CfgFile& cfg, GEPNet **in_nets )
{
    config = &cfg;

    // NETWORKS
    nets = new GEPNet*[config->number_of_outputs];
    for( int i = 0; i < config->number_of_outputs; i++ )
        nets[i] = in_nets[i];

    // OUTPUTS
    outputs = new double[config->number_of_outputs];
}

MIMO_FF_GEPNet::~MIMO_FF_GEPNet()
{
    delete [] nets;    
    delete [] outputs;
}

const double* MIMO_FF_GEPNet::evaluate( double *ivalues )
{
    for( int i = 0; i < config->number_of_outputs; i++ )
        outputs[i] = nets[i]->evaluate( ivalues );

    return (const double*)outputs;
}


#endif

