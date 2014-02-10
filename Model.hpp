
#ifndef MODEL_H
#define MODEL_H

#include <ostream>
#include "CfgFile.hpp"
#include "Rand.hpp"
#include "PPC.hpp"
#include "Individual.hpp"
#include "BitwiseWeights.hpp"
#include "DataCollector.hpp"
#include "CGPModel.hpp"

class DataCollector;

class Model
{
private:
    static CfgFile *config;
    DataCollector *dc;
    //RandomNumberGenerator rng;
    Individual **population;

    // GEP
    PrototypeChromosome **ppc_array;
	Neuron **inputs;
	Individual* create_pipe_gep_individual( double **weight_arrays, bool with_gm, RandomNumberGenerator& rng );

	// CGP
	CGPModel cgp_model;

	// SHC Weight
	SHCWeightCollection **shc_weight_array;

    // Bitwise Weight
    BitwiseWeights *bitwise_weights;


public:
    Model();
    ~Model();
    void init();
    void set_data_collector( DataCollector& dc );
    static void set_config( CfgFile& cfg );
    Neuron** get_inputs();

    void init_population();
	void delete_population();
    void sample( RandomNumberGenerator& rng );
    void measure_fitness( ostream& fout );
    void adapt( Individual *indiv );
    void mutate( RandomNumberGenerator& rng );

	bool stop_condition_met();

    void print( ostream& fout );

    Individual* get_elite();
    Individual* get_best();
	Individual* get_gm_indiv();

    int total_fitness, highest_fitness, lowest_fitness, elite_fitness;
    int highest_fitness_index, lowest_fitness_index;
    int total_evals;
    Individual *elite_individual, *best_individual;
	bool is_elitist_learning;
};

#endif

