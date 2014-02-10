/*
 *  Rand.hpp
 *	Wrapper for rand.h (written in C) for C++.
 *
 */

#ifndef PDNE_RAND_H
#define PDNE_RAND_H

class RandomNumberGenerator
{
private:
	static const int MAX_STREAMS = 256;
	static bool inUse[MAX_STREAMS];
	int streamid;
	void init( long initValue );
public:
    RandomNumberGenerator();
	RandomNumberGenerator( long initValue );
	~RandomNumberGenerator();
	double getRandom();
	double getNormal( double mean, double stddev );
	double getUniform( double start, double end );
	long getEquilikely( long start, long end );
	void setSeed( long new_seed );
	long getSeed();
	long getCurrentSeed();
	long getMax();
};

#endif


