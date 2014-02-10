
#ifndef EXPERIMENT
#define EXPERIMENT

#include "FitnessFunctions.hpp"
#include "PoleBalancing.hpp"
#include "DoublePole.hpp"
#include "PPC.hpp"
#include "Rand.hpp"
#include "Experiment.hpp"
#include "Individual.hpp"
#include "Model.hpp"
#include "DataCollector.hpp"

#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <cmath>
#include <string>
#include <sstream>

CfgFile* Experiment::config = NULL;

void Experiment::set_config( CfgFile& cfg )
{
    config = &cfg;
}

Experiment::Experiment()
{
    if( config == NULL )
        DataCollector::debug << "Experiment::config is NULL." << endline;
}

Experiment::~Experiment()
{
}

void Experiment::run( DataCollector& dc, RandomNumberGenerator& rng )
{
    // INITIALIZE
	double prob_learning_type;  // ELITE or BEST OF GENERATION
	int i;

	for( int run = 0; run < config->number_of_runs; run++ )
	{
        std::cout << "" << (run+1) << ",";
        std::cout.flush();

        dc.start_new_run( run );

		prob_learning_type = 1.;

        dc.run << "Run seed = " << rng.getCurrentSeed() << endline;

        //INITIALIZE THE MODEL
        Model model;
        model.set_data_collector( dc );

        // RUN FOR N GENERATIONS
		for( i = 0; i < config->number_of_generations; i++ )
		{
            //std::cout << (run+1) << "." << i << "," << std::flush;

			// ELITIST LEARNING
			if(     config->structural_learning == PIPE
                &&  prob_learning_type < config->pipe_prob_elitist_learn )
			{
				model.is_elitist_learning = true;
				model.adapt( model.get_elite() );
			}
			else
			{
				// BEST OF LEARNING
				// GENERATION BASED LEARNING
                model.sample( rng );

				if(		config->fitness_test == RETINA_SWITCHING 
					&&	( i % config->retina_switch_after ) == 0 
					&&	i != 0 )
					retina_test_switch();

                // MEASURE FITNESS + INDEX BEST and ELITE
                model.measure_fitness( dc.run );

				// ADAPT PPC TOWARD BEST
                model.adapt( model.get_best() );

			}

            // MUTATE
			model.mutate( rng );

			prob_learning_type = rng.getRandom();

            dc.log_generation( run, i, model );

			dc.flush_debug();
			if(		( model.stop_condition_met() && config->stop_on_target_reached )
                ||	( i + 1 ) == config->number_of_generations )
			{
			    dc.log_run( run, i, model );
				break;
			}

		}



		// RANDOM FITNESS TEST (to confirm findings)
        dc.log_random_fitness( model );
		
	}

    dc.log_experiment();
}

#endif

