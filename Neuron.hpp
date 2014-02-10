#ifndef NEURON_H
#define NEURON_H

#include "CfgFile.hpp"
#include "ActivationFunctions.hpp"

class Neuron
{
private:
	short num_inputs;
	double *weights;
	double threshold;
	Neuron **inputs;
	double value;
	void initWeights();
	void initInputs();
	
public:
    static CfgFile *config;
    static void set_config( CfgFile& cfg );
    Neuron();
	Neuron( int num );
	Neuron( int num, double *weights, int num_weights );
	~Neuron();
	void init( int num );
	double evaluate();
	void setWeights( double *sw, int nw );
	void setWeight( short i, double weight );
	double getWeight( short i );
	void setValue( double val );
	double getValue();
	void setLink( short i, Neuron *n );
	bool isInput();
	short getNumInputs();
	bool isOrNeuron;
};




#endif

