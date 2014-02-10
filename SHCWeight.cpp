
#ifndef SHC_WEIGHT
#define SHC_WEIGHT

#include "SHCWeight.hpp"
#include "GEPNN.hpp"
#include <iostream>
#include <fstream>


SHCWeight::SHCWeight( double mean, double stddev, double p_mean_move = 0.05, double p_std_dev_reduce = 0.997241 )
{
	mean_move = p_mean_move;
	std_dev_reduce = p_std_dev_reduce;
	this->mean = mean;
	this->stddev = stddev;
	num_best_weights = 0;
	for( int i = 0; i < MAX_BEST_WEIGHTS; i++ )
	{
		best_weights[i].fitness = 0;
		best_weights[i].weight = -29.7;
	}
}

SHCWeight::~SHCWeight()
{
}

double SHCWeight::getWeight( RandomNumberGenerator& rng )
{    
	return rng.getNormal( mean, stddev );
}

double SHCWeight::getMean()
{
	return mean;
}

double SHCWeight::getStddev()
{
	return stddev;
}

void SHCWeight::updateMeanAndStdDev()
{
	double middle = 0.;
	for( int i = 0; i < num_best_weights; i++ )
		middle += best_weights[i].weight;
	middle /= num_best_weights;

	mean += mean_move * ( middle - mean );

	stddev *= std_dev_reduce;
}

void SHCWeight::setBestWeight( double w, int f )
{
	int index = -1;

	// IF BEST_WEIGHTS NOT FULL ADD THE FITNESS
	if( num_best_weights < MAX_BEST_WEIGHTS )
	{
		index = num_best_weights;
		num_best_weights++;
	}
	else
	{
	// IF BEST_WEIGHTS IS FULL, FIND THE ONE WITH THE BIGGEST FITNESS (worst fitness)
		int biggest = 0;
		for( int i = 0; i < MAX_BEST_WEIGHTS; i++ )
		{
			if( best_weights[i].fitness < best_weights[biggest].fitness )
				biggest = i;
		}

		if( f > best_weights[biggest].fitness )
		{
			index = biggest;
		}
	}

	if( index >= 0 )
	{
		best_weights[index].fitness = f;
		best_weights[index].weight  = w;
	}

	updateMeanAndStdDev();
}

void SHCWeight::print( std::ostream &fout )
{
	using namespace std;

	fout << " { ";
	fout.precision(2);
	for( int i = 0; i < num_best_weights; i++ )
		fout << "{" << fixed << best_weights[i].weight << ", " << best_weights[i].fitness << "}, ";
	fout << "mean= " << fixed << mean << " std_dev= " << fixed << stddev << " }";
}


#endif

