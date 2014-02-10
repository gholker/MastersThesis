#ifndef DATA_COLLECTOR_HPP
#define DATA_COLLECTOR_HPP

#include <string>
#include <sstream>
#include <iostream>

#include "DataCollectionDataTypes.h"
#include "CfgFile.hpp"
#include "Experiment.hpp"
#include "Model.hpp"
#include "Tools.hpp"

class Experiment;
class Model;


int get_file_number( string fn );

class DataCollector
{
private:
    static CfgFile *config;
    string filename, base_filename;
    int filenumber;
	string fname_main, fname_csv, fname_run;

    generation_data **generation_info;
    run_data *run_info;
    experiment_data experiment_info;
	bool suppress_output;
public:
    stringstream main, csv, run;
	static stringstream debug;

    static void set_config( CfgFile& cfg );
    void init_data();
    void delete_data();

	void output_buffer( string buffer, string filename );

    DataCollector( string fn );
    ~DataCollector();

	void flush_debug();

    void log_main( string in );
    void log_run( string in );
    void log_csv( string in );

	double get_success_rate();
	void suppress();

    void start_new_run( int run_num );
    void log_generation( int run_num, int generation, Model& model );
    void log_experiment();
    void log_run( int run_num, int number_of_generations, Model& model );
    void log_random_fitness( Model& model );
	void create_matlab_plot( generation_data *to_plot, int num_generation, int num_bins );
	void log_summary_table( double mean, double best, double worst, double std_dev, int failures );
	void log_latex_figures( int *bin_info );
	void log_bins();
};

#endif
