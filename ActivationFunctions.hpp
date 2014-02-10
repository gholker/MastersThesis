#ifndef ACTIVATION_FUNCTIONS_HPP
#define ACTIVATION_FUNCTIONS_HPP

double fsigmoid( double x );
double fsigmoid( double x, double slope );
double binary_activation( double sum, double threshold );
double hyperbolic_tangent( double x, double slope );

#endif

