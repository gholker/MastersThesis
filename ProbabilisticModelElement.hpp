#ifndef PROB_MODEL_ELEM_H
#define PROB_MODEL_ELEM_H

#include "CfgFile.hpp"
#include "Rand.hpp"
#include <iostream>

class ProbabilisticModelElement
{
private:
    static CfgFile *config;
    int number_of_probabilities;
    double *probabilities;
public:
    static void set_config( CfgFile& cfg );

    ProbabilisticModelElement();
    ProbabilisticModelElement( int num );
    ~ProbabilisticModelElement();
    void init( int num );
    void delete_probabilities();
	void print( ostream& fout );
	int get_length();

	// LEARNING
    int get_value( RandomNumberGenerator& rng );
    double get_probability( int index );
    void normalize( int index );
	void normalize_fix();
    void pipe_mutate( int size, RandomNumberGenerator& rng );
    void pipe_increment( int index );
	void pbil_increment( int index );
	void pbil_mutate( RandomNumberGenerator& rng );
};


#endif
