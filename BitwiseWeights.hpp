#ifndef BITWISE_WEIGHTS_HPP
#define BITWISE_WEIGHTS_HPP

#include "CfgFile.hpp"
#include "ProbabilisticModelElement.hpp"
#include "BitwiseWeight.hpp"

class BitwiseWeights
{
private:
    int number_of_weights;
    BitwiseWeight *weights;
    bitwise_weight_value* weight_values;
    static CfgFile *config;
public:
    static void set_config( CfgFile& cfg );

    BitwiseWeights();
    BitwiseWeights( int num );
    ~BitwiseWeights();

    void init( int num );
    void delete_weights();
    void delete_weight_values();
    void print( std::ostream& out );

    bitwise_weight_value* get_weights( RandomNumberGenerator& rng );

    void pipe_increment( bitwise_weight_value* values );
	void pipe_adapt( bitwise_weight_value* values, int fitnessBest, int fitnessElite );
    void pipe_mutate( RandomNumberGenerator& rng );

	void pbil_increment( bitwise_weight_value* values );
	void pbil_adapt( bitwise_weight_value* values );
    void pbil_mutate( RandomNumberGenerator& rng );

    void normalize( bitwise_weight_value* values );
    double get_probability( bitwise_weight_value* values );
	void guided_mutation( bitwise_weight_value *elite_values, bitwise_weight_value *new_values, RandomNumberGenerator& rng );
};

#endif
