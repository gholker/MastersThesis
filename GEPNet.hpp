#ifndef NEURALNET_H
#define NEURALNET_H

#include "Neuron.hpp"
#include "CfgFile.hpp"

class GEPNet
{
protected:
	Neuron *root;
	short num_inputs;
	Neuron **inputs;
	bool should_delete;
public:
	GEPNet( Neuron *rt, Neuron **inps, int num_inputs );
	~GEPNet();
	double evaluate();
	double evaluate( double *ivalues );
	void setInputs( Neuron **inps, int num_inputs );
	Neuron* getRoot();
};

class MIMO_FF_GEPNet
{
private:
    GEPNet **nets;
    double *outputs;
    CfgFile *config;
public:
    MIMO_FF_GEPNet( CfgFile& cfg, GEPNet **in_nets );
	~MIMO_FF_GEPNet();
	const double* evaluate( double *ivalues );
	double evaluate( double val );
};


#endif

