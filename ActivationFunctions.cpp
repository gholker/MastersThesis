#ifndef ACTIVATION_FUNCTIONS
#define ACTIVATION_FUNCTIONS

#include "ActivationFunctions.hpp"
#include <cmath>

double fsigmoid( double x )
{
	return fsigmoid( x, 4.924273 );
}

// Modified from NEAT code.
// The slope makes the sigmoid 0 at x=-1 and 1 at x=1
double fsigmoid( double x, double slope = 4.924273 )
{
	return 1 / ( 1 + exp( -slope * x ));
}

// The slope of 4.924273 makes this -1 at x = -1 and 1 at x = 1
double hyperbolic_tangent( double x, double slope )
{
	return tanh( slope * x );
}

double binary_activation( double sum, double threshold )
{
	if( sum >= threshold )	return 1.0;
    else					return 0.0;
}


#endif

