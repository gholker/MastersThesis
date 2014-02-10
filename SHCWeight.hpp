
#ifndef SHC_WEIGHT_H
#define SHC_WEIGHT_H

#include "Rand.hpp"
#include "GEPNN.hpp"
#include <fstream>

typedef struct best_weight
{
	double weight;
	int fitness;
} best_weight;

class SHCWeight
{
private:
	static const short MAX_BEST_WEIGHTS = 3;
	double mean_move;
	double std_dev_reduce;
	best_weight best_weights[MAX_BEST_WEIGHTS];
	short num_best_weights;
	double mean;
	double stddev;
	
public:
	SHCWeight( double mean, double stddev, double p_mean_move, double p_std_dev_reduce );
	~SHCWeight();
	double getWeight( RandomNumberGenerator& rng );
	double getMean();
	double getStddev();
	void setBestWeight( double w, int f );
	void updateMeanAndStdDev();
	void print( std::ostream &fout );
};

#endif

