

#ifndef CFGFILE_H
#define CFGFILE_H

#include "Enumerations.h"

#include <fstream>
#include <string>

using namespace std;

class CfgFile
{
private:
public:
	CfgFile( char *file_name );
	~CfgFile();
	void print( std::ostream &fout );
	string to_string();
	void evaluate( char token[100], char value[100] );
	bool default_exists();
	void readFile( const char *file_name );
    void set_defaults();

    bool is_PIPE_GEP();

	string default_filename;
	string filename;
	string experiment_name;
	string matlab_filename, summary_table_filename, latex_figures_filename, success_rate_table_filename;

	// Experiment
	int stop_on_target_reached;
	int retina_switch_after;
	int number_of_experiments;

	// FITNESS
	short fitness_test;
	short fitness_direction;
	int fitness_target;

    // GUIDED MUTATION
	double gm_prob_of_mutation;
	double gm_population_percent;
	int gm_on_best;

	// GEP
	short gep_min_arity;
	short gep_max_arity;
	short gep_head_length;
	short gep_num_genes;
	short gep_num_weights;

	// GENERIC
	short output_csv, output_run;
	short population_size;
	short number_of_inputs;
	short number_of_outputs;
	int number_of_generations;
	short number_of_runs;
	long seed, pole_seed;
	short representation, weight_representation, structural_learning, weight_learning, with_guided_mutation;
	int number_of_weight_arrays, number_of_weights;
	int number_of_bins;

	// NEURAL NETWORK
	short activation_function;
	double activation_slope;

    // STOCHASTIC HILL CLIMBING
	double shc_std_dev_reduce, shc_mean_move;

	// PIPE
	double  pipe_prob_elitist_learn, pipe_learning_rate, pipe_fitness_constant,
            pipe_prob_mutation, pipe_mutation_rate;

	// PBIL
	double pbil_learning_rate, pbil_mutation_probability, pbil_mutation_shift;

	// BITWISE
	double bw_range;

    // CGP
    short cgp_arity;
    short cgp_number_of_functions;
	short cgp_number_of_parsed_functions;
    short *cgp_functions;
    short cgp_number_of_rows, cgp_number_of_columns;
    short cgp_levels_back;
	short cgp_num_weights, cgp_weight_redirect;
	int get_cgp_length();
	int get_cgp_num_weights();
	int get_cgp_weight_index( int in, int out );
	int get_cgp_number_of_possibilities( int column );
	int get_cgp_number_of_inaccessible_nodes( int column );
};

#endif

