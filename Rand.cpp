/*
 *  Rand.cpp
 *	Wrapper for rand.h (written in C) for C++.
 *
 */
#ifndef PDNE_RAND
#define PDNE_RAND

#include "rand.h"
#include <iostream>

#include "Rand.hpp"


bool RandomNumberGenerator::inUse[RandomNumberGenerator::MAX_STREAMS] = { false };

RandomNumberGenerator::RandomNumberGenerator()
{
    init( -1L );
}

RandomNumberGenerator::RandomNumberGenerator( long initValue = -1L ) // -1 means we generate random seed from system time
{
    init( initValue );
}

void RandomNumberGenerator::init( long initValue = -1L )
{
    int i;

	// find first unused stream
	for( i = 0; i < MAX_STREAMS; i++ )
		if( !inUse[i] )
			break;

	if( i == MAX_STREAMS ) // NO UNUSED STREAM FOUND
	{
		streamid = -1;
	}
	else
	{
		streamid = i + 1;;
		inUse[i] = true;
		SelectStream( streamid );
		PutSeed( initValue );
	}
}

RandomNumberGenerator::~RandomNumberGenerator()
{
	if( streamid > 0 )
		inUse[streamid - 1] = false;
}

double RandomNumberGenerator::getRandom()
{
	if( streamid > 0 )
	{
		SelectStream( streamid );
		return Random();
	} else
		return 0.;
}

double RandomNumberGenerator::getNormal( double mean, double stddev )
{
	if( streamid > 0 )
	{
		SelectStream( streamid );
		return Normal( mean, stddev );
	} else
		return 0.;
}

double RandomNumberGenerator::getUniform( double start, double end )
{
	if( streamid > 0 )
	{
		SelectStream( streamid );
		return Uniform( start, end );
	} else
		return 0.;
}

long RandomNumberGenerator::getEquilikely( long start, long end )
{
	if( streamid > 0 )
	{
		SelectStream( streamid );
		return Equilikely( start, end );
	} else
		return 0L;
}

long RandomNumberGenerator::getSeed()
{
	return getEquilikely( 1L, MODULUS - 1 );  // MODULUS IS DEFINED IN rand.h AS 2147483647
}

void RandomNumberGenerator::setSeed( long new_seed )
{
    if( streamid > 0 )
	{
		SelectStream( streamid );
		PutSeed( new_seed );
	} else
		std::cout << "Error setting seed." << std::endl;
}

long RandomNumberGenerator::getCurrentSeed()
{
	if( streamid > 0 )
	{
		SelectStream( streamid );
		return seed[streamid];
	} else
		return -1L;
}

long RandomNumberGenerator::getMax()
{
	return MODULUS;
}

#endif
