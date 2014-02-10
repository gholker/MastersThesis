
#ifndef ENUMS
#define ENUMS

const char endline = '\n';

enum
{
    MAX_FITNESS = 2147483647
};

enum
{
    FITNESS_UP = 0,
    FITNESS_DOWN = 1
};

// FITNESS ENUMS
enum
{
	XOR = 1,
	SIX_BIT = 2,
	POLE_BALANCING = 3,
	DOUBLE_POLE_BALANCING = 4,
	RETINA = 5,
	RETINA_SWITCHING = 6
};

enum
{
    NO = 0,
    YES = 1
};

enum
{
    PIPE = 1, // should be one so that if( param ) can be used.
    GEP,
    STOCHASTIC_HILL_CLIMBING,
    SIGMOID,
    BINARY,
	HYPERBOLIC_TANGENT,
    BITWISE,
    CGP,
	PBIL
};

enum
{
	PBIL_MUTATE,
	PBIL_INCREMENT,
	PIPE_MUTATE,
	PIPE_INCREMENT,
	GET_PROBABILITY,
	NORMALIZE,
	GUIDED_MUTATION,
	GET_GENOTYPE,
	PRINT,
	INIT_MODEL
};

#endif
