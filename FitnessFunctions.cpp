
#ifndef FITNESS
#define FITNESS

#include "FitnessFunctions.hpp"
#include <iostream>
#include <fstream>

using namespace std;

// XOR FUNCTION FITNESS
int fitnessXOR( Individual *net, std::ostream &fout, bool printoutput )
{
	int fitness = 0;
	double inputs[3];
	inputs[2] = 1.;

	

	inputs[0] = 0.;
	inputs[1] = 0.;

	if( *net->evaluate( inputs ) == 0. )
	{
		if( printoutput ) fout << "0,0 - GOOD\n";
		fitness++;
	}

	inputs[0] = 1.;
	inputs[1] = 0.;

	if( *net->evaluate( inputs ) == 1. )
	{
		if( printoutput ) fout << "1,0 - GOOD\n";
		fitness++;
	}

	inputs[0] = 0.;
	inputs[1] = 1.;

	if( *net->evaluate( inputs ) == 1. )
	{
		if( printoutput ) fout << "0,1 - GOOD\n";
		fitness++;
	}

	inputs[0] = 1.;
	inputs[1] = 1.;

	if( *net->evaluate( inputs ) == 0. )
	{
		if( printoutput ) fout << "1,1 - GOOD\n";
		fitness++;
	}

	//net->print( std::cout );

	return fitness;
}

int fitness6BitMux( Individual *net, std::ostream &fout, bool printoutput )
{
	int fitness = 0;
	int i,j,k;
	char index;
	double value;
	double t;
	short num_inputs = 6;
	double *inputs = new double[num_inputs+1];
	inputs[num_inputs] = 1.; // BIAS

	for( i = 0; i < 64; i++ )
	{
		index = i >> 4; // THE ADDRESS PUT INTO THE MUX
		value = (((i & 0x0F) & (0x1 << index)) > 0) ? 1. : 0.; // THE VALUE AT THAT ADDRESS
		// CREATE INPUTS ARRAY
		for( j = num_inputs - 1, k = 0; j >= 0; j--, k++ )
			inputs[k] = ((i & (0x01 << j)) > 0) ? 1. : 0.;
		// EVALUATE NETWORK
		t = *net->evaluate( inputs );

		if( t == value )
		{
			if( printoutput )
			{
				fout.precision(2);
				for( j = 0; j < num_inputs; j++ )
					fout << inputs[j] << ", ";
				fout << "= " << value << endl;
			}
			fitness++;
		}
		else
		{
			if( printoutput )
			{
				fout.precision(2);
				for( j = 0; j < num_inputs; j++ )
					fout << inputs[j] << ", ";
				fout << "= " << t << " X" << endl;
			}
		}
	}
	delete [] inputs;
	return fitness;
}

bool retina_test_and = true;

void retina_test_switch()
{
	retina_test_and = !retina_test_and;
}

int fitnessRetina( Individual *net, std::ostream &fout, bool printoutput )
{
	short num_inputs = 8;
	double *inputs = new double[num_inputs+1];
	inputs[num_inputs] = 1.; // BIAS
	int count = 0;	

	for( int c = 0; c < 256; c++ )
	{		
		for( int i = 0; i < 8; i++ )
			inputs[i] = ( ( ( c & ( 0x1 << i ) ) == 0 ? 0. : 1. ) );
		int count_left = 0, count_right = 0;
		for( int i = 0; i < 4; i++ )
		{
			if( inputs[4+i] == 1. )
				count_left++;
			if( inputs[i] == 1. )
				count_right++;
		}
		bool is_left = false, is_right = false;
		if(		count_left >= 3 
			|| ( count_left == 2 && inputs[7] == 1. && inputs[6] == 1. )
			|| ( count_left == 1 && ( inputs[7] == 1. || inputs[6] == 1. ) ) )
			is_left = true;
		if(		count_right >= 3 
			|| ( count_right == 2 && inputs[1] == 1. && inputs[0] == 1. )
			|| ( count_right == 1 && ( inputs[1] == 1. || inputs[0] == 1. ) ) )
			is_right = true;
		bool correct_output = ( retina_test_and ? is_left && is_right : is_left || is_right );
		double response = ( net->evaluate( inputs ) )[0];
		if(		(  correct_output && response == 1. )
			||	( !correct_output && response == 0. ) )
			count++;

		if( printoutput )
		{
			fout << "c=" << c << "correct=" << correct_output << endline;
			fout << "" << (int)inputs[6] << " " << (int)inputs[4] << " " << (int)inputs[2] << " " << (int)inputs[0] << endline;
			fout << "" << (int)inputs[7] << " " << (int)inputs[5] << " " << (int)inputs[3] << " " << (int)inputs[1] << endline;
		}
	}
	delete [] inputs;
	return count;
}

#endif

