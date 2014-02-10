#ifndef SHC_WEIGHT_COLLECTION_HPP
#define SHC_WEIGHT_COLLECTION_HPP

#include <iostream>
#include "GEPNN.hpp"
#include "SHCWeight.hpp"
#include "Individual.hpp"

class SHCWeightCollection
{
private:
	SHCWeight **weights;
	short size;
public:
	SHCWeightCollection(   short size,
                        double range,
                        double mean,
                        double stddev,
                        double p_mean_move,
                        double p_std_dev_reduce );
	~SHCWeightCollection();
	double* getWeights( RandomNumberGenerator& rng );
	void setBestWeights( int index, Individual *indiv );
	void print( std::ostream &fout );
};

#endif
