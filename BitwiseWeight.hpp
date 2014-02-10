#ifndef BITWISE_WEIGHT_HPP
#define BITWISE_WEIGHT_HPP

#include <iostream>
#include "ProbabilisticModelElement.hpp"
#include "CfgFile.hpp"

typedef struct
{
    double decimal;
    int integer;
	bool is_used;
} bitwise_weight_value;

class BitwiseWeight
{
private:
    int number_of_elements;
    ProbabilisticModelElement *elements;

	static CfgFile *config;
public:
	static void set_config( CfgFile&  cfg );

    BitwiseWeight();
    ~BitwiseWeight();

    void init( int num );
    void delete_elements();
    void print( ostream& out );

	double get_decimal_value( int int_value );
    bitwise_weight_value get_weight( RandomNumberGenerator& rng );
    
	void pipe_increment( int int_weight );
    void pipe_mutate( int size, RandomNumberGenerator& rng );

	void pbil_increment( int int_weight );
    void pbil_mutate( RandomNumberGenerator& rng );

    void normalize( int int_weight );
    double get_probability( int int_weight );
	bitwise_weight_value guided_mutation( bitwise_weight_value elite_value, bitwise_weight_value new_value, RandomNumberGenerator& rng );
};

#endif
