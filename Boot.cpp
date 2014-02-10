#ifndef BOOT
#define BOOT

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <cmath>

#include "Experiment.hpp"
#include "Model.hpp"
#include "Individual.hpp"
#include "PoleBalancing.hpp"
#include "CfgFile.hpp"
#include "DataCollector.hpp"
#include "ProbabilisticModelElement.hpp"
#include "BitwiseWeights.hpp"
#include "BitwiseWeight.hpp"
#include "CGPModel.hpp"
#include "CGPGraph.hpp"
#include "PPC.hpp"
#include "FitnessFunctions.hpp"

int main( int argc, char* argv[] )
{

	RandomNumberGenerator rng;

    for( int arg = 1; arg < argc; arg++ )
    {

        // LOAD CONFIG FILE
        CfgFile cfg( argv[arg] );
        std::cout << endline << "FILE: " << argv[arg] << endline;

        // Share the Config with different objects that need it
        Neuron::set_config( cfg );
        Individual::set_config( cfg );
        Experiment::set_config( cfg );
        Model::set_config( cfg );
        DataCollector::set_config( cfg );
        ProbabilisticModelElement::set_config( cfg );
        BitwiseWeights::set_config( cfg );
        BitwiseWeight::set_config( cfg );
        CGPModel::set_config( cfg );
		CGPModelNode::set_config( cfg );
		CGPGraph::set_config( cfg );
		PrototypeChromosome::set_config( cfg );

		
        // SET SEED for Pole Balancing
        // harmless even when PB is not the fitness test
        // i.e. no memory is allocated
		pole_balance_set_seed( cfg.pole_seed );
		
		// CREATE RANDOM NUMBER GENERATOR
        rng.setSeed( cfg.seed );

		double *success_rates = new double[cfg.number_of_experiments];
		double mean_success_rate = 0, best_success_rate = 0, worst_success_rate = 1.1;

		for( int exp = 0; exp < cfg.number_of_experiments; exp++ )
		{
			// START OUTPUT FILE
			DataCollector dc( cfg.filename );
			if( exp != 0 && cfg.number_of_experiments > 1 )
				dc.suppress();
		
			// OUTPUT CONFIGURATION INFORMATIONs
			dc.main << "Experiment Starting -- " << argv[arg] << endline
					<< "seed = " << rng.getCurrentSeed() << endline
					<< cfg.to_string();

			// create experiment object
			Experiment e;

			e.run( dc, rng );

			success_rates[exp] = dc.get_success_rate();
			mean_success_rate += dc.get_success_rate();
			if( success_rates[exp] > best_success_rate )
				best_success_rate = success_rates[exp];
			if( success_rates[exp] < worst_success_rate )
				worst_success_rate = success_rates[exp];
		}
		if( cfg.number_of_experiments > 1 )
		{

			mean_success_rate /= cfg.number_of_experiments;
			double std_dev = 0;
			for( int i = 0; i < cfg.number_of_experiments; i++ )
			{
				std_dev +=      ( success_rates[i] - mean_success_rate )
							*   ( success_rates[i] - mean_success_rate );
			}
			std_dev /= cfg.number_of_experiments;
			std_dev = sqrt( std_dev );

			DataCollector dc( cfg.filename );
			dc.log_summary_table( 100*mean_success_rate, 100*best_success_rate, 100*worst_success_rate, 100*std_dev, -1 );
		}
		delete [] success_rates;
    }

	return 0;
}

#endif
	