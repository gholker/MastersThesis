
#ifndef SHC_WEIGHT_COLLECTION
#define SHC_WEIGHT_COLLECTION

#include "SHCWeightCollection.hpp"


SHCWeightCollection::SHCWeightCollection( short size, double range, double mean = 0., double stddev = 1., double p_mean_move = 0.05, double p_std_dev_reduce = 0.997241 )
{
    this->size = size;
	weights = new SHCWeight*[this->size];
	for( int i = 0; i < this->size; i++ )
		weights[i] = new SHCWeight( mean, stddev, p_mean_move, p_std_dev_reduce );
}

SHCWeightCollection::~SHCWeightCollection()
{
	for( int i = 0; i < size; i++ )
		delete weights[i];
	delete [] weights;
}

double* SHCWeightCollection::getWeights( RandomNumberGenerator& rng )
{
	double *w = new double[size];
	for( int i = 0; i < size; i++ )
	{
		w[i] = weights[i]->getWeight( rng );
	}
	return w;
}

void SHCWeightCollection::setBestWeights( int index, Individual *indiv )
{
	double *w = indiv->get_weights( index );
	int f = indiv->fitness;
	bool *weights_used = indiv->get_weights_used( index );

	for( int i = 0; i < size; i++ )
		if( !weights_used || ( weights_used && weights_used[i] ) )
			weights[i]->setBestWeight( w[i], f );
}

void SHCWeightCollection::print( std::ostream &fout )
{
	using namespace std;
	for( int i = 0; i < size; i++ )
	{
		fout << "weight[" << i << "] = ";
		weights[i]->print( fout );
		fout << endl;
	}
}

#endif
