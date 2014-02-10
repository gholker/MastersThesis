#ifndef DATA_COLLECTOR
#define DATA_COLLECTOR

#include "DataCollector.hpp"
#include "PoleBalancing.hpp"
#include "FitnessFunctions.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


int get_file_number( string fn )
{
	using namespace std;
	ifstream fin;
	int i;
    string newfn;

    i = 0;
	while( true )
	{
	    newfn = fn + ".";
	    newfn += int_to_string( i );
	    newfn += ".out.txt";
		fin.open( newfn.c_str() );
		if( fin.fail() )
			break;
		fin.close();
		i++;
	}
	return i;
}

void DataCollector::set_config( CfgFile& cfg )
{
    config = &cfg;
}

CfgFile* DataCollector::config = NULL;
stringstream DataCollector::debug;

DataCollector::DataCollector( string fn )
{
	suppress_output = false;
	base_filename = fn;
    stringstream ss;	
    filenumber = get_file_number( fn );
    ss << fn << "." << int_to_string( filenumber );
    filename = ss.str();
    ss << ".out.txt";
	fname_main = ss.str();
    init_data();
}

void DataCollector::flush_debug()
{
	output_buffer( debug.str(), "debug.log" );
	debug.str( "" );
}

DataCollector::~DataCollector()
{
	
	output_buffer( main.str(), fname_main );
	if( config->output_csv == YES )
		output_buffer( csv.str(), fname_csv );
	if( config->output_run == YES )
		output_buffer( run.str(), fname_run );

    delete_data();
}

void DataCollector::output_buffer( string buffer, string filename )
{
	if( !suppress_output )
	{
		ofstream fout;
		fout.open( filename.c_str(), ios_base::app );
		fout << buffer;
		fout.flush();
		fout.close();
	}
}

void DataCollector::init_data()
{
    experiment_info.total_evals = 0;
    experiment_info.average_fitness = 0.;
    experiment_info.best_evals = 0;
    experiment_info.worst_evals = 0;
    experiment_info.average_evals = 0.;
    experiment_info.standard_deviation_evals = 0.;

    run_info = new run_data[config->number_of_runs];
    for( int i = 0; i < config->number_of_runs; i++ )
    {
        run_info[i].run_number = i;
        run_info[i].number_of_generations = 0;
        run_info[i].elite_fitness = 0;
        run_info[i].elite_size = 0;
        run_info[i].number_of_evals = 0;
    }

    this->generation_info = new generation_data*[config->number_of_runs];
    for( int i = 0; i < config->number_of_runs; i++ )
    {
        this->generation_info[i] = new generation_data[config->number_of_generations];
        for( int j = 0; j < config->number_of_generations; j++ )
        {
            generation_info[i][j].generation_number = -1;
            generation_info[i][j].total_fitness     = 0;
            generation_info[i][j].highest_fitness   = 0;
            generation_info[i][j].lowest_fitness    = 0;
            generation_info[i][j].elite_fitness     = 0;
            generation_info[i][j].average_fitness   = 0;
            generation_info[i][j].number_of_evals   = 0;
			generation_info[i][j].number_of_runs	= 0;
        }
    }
}

void DataCollector::delete_data()
{
    delete [] run_info;
    for( int i = 0; i < config->number_of_runs; i++ )
        delete [] generation_info[i];
    delete [] generation_info;
}

void DataCollector::start_new_run( int run_num )
{
    stringstream ss;
	if( config->output_csv == YES )
		output_buffer( csv.str(), fname_csv );
	if( config->output_run == YES )
		output_buffer( run.str(), fname_run );

    ss << filename << "." << run_num << ".out.txt";
    fname_run = ss.str();
	run.str( "" );

    ss.str( "" );
    ss << filename << "." << run_num << ".csv";
    fname_csv = ss.str();
	csv.str( "" );

    csv << "Generation,Total,Highest,Lowest,Elite,Average,Spread" << endline;
}

void DataCollector::log_main( string in )
{
    main << in;
}

void DataCollector::log_run( string in )
{
    run << in;
}

void DataCollector::log_csv( string in )
{
	csv << in;
}

void DataCollector::log_generation( int run_num, int generation, Model& model )
{
	run << endline;
	if( model.is_elitist_learning )
	{
		run << "<Elitist Learning>" << endline;
		model.get_elite()->print( run );
	}
	else
		model.get_best()->print( run );
    model.print( run );

    generation_data gen;
	gen.total_fitness = model.total_fitness;
    gen.elite_fitness = model.elite_fitness;
    gen.lowest_fitness = model.lowest_fitness;
    gen.highest_fitness = model.highest_fitness;
    gen.average_fitness = (int)( (double)model.total_fitness / (double)config->population_size + .5 );
    gen.generation_number = generation + 1;

   this->csv   << gen.generation_number << ','
               << gen.total_fitness << ','
               << gen.highest_fitness << ','
               << gen.lowest_fitness  << ','
               << gen.elite_fitness   << ','
               << gen.average_fitness << ','
               << ( gen.highest_fitness - gen.lowest_fitness ) << endline;

   if( generation - 1 >=  0 && generation_info[run_num][generation-1].elite_fitness == config->fitness_target )
	   gen.generation_number = -1;
   generation_info[run_num][generation] = gen;
}

void DataCollector::create_matlab_plot( generation_data *to_plot, int num_generations, int num_bins )
{
	if( num_bins > 0 )
	{
		stringstream ss;
		stringstream gen, best, avg, worst, elite;
		gen		<< "generation		= [";
		best	<< "best_fitness	= [";
		avg		<< "avg_fitness		= [";
		worst	<< "worst_fitness	= [";
		elite	<< "elite_fitness	= [";
		for( int i = 0; i < num_generations; i++ )
		{
			gen << to_plot[i].generation_number;
			best << to_plot[i].highest_fitness;
			avg << to_plot[i].average_fitness;
			worst << to_plot[i].lowest_fitness;
			elite << to_plot[i].elite_fitness;
			if(		i != num_generations - 1
				&&	to_plot[i+1].total_fitness > 0 )
			{
				gen << ",";
				best << ",";
				avg << ",";
				worst << ",";
				elite << ",";
			}
			else
				break;
		}
		gen << "];" << endline;
		best << "];" << endline;
		avg << "];" << endline;
		worst << "];" << endline;
		elite << "];" << endline;

		ss << "% NUMBER OF RUNS IN THE BIN: " << num_bins << endline << endline;
		ss << gen.str() << best.str() << avg.str() << worst.str() << elite.str() << endline;

		ss << endline;
		ss << "best_plot	= plot( generation, best_fitness,':s','color','black', 'MarkerFaceColor','black'); hold on;" << endline;
		ss << "avg_plot		= plot( generation, avg_fitness,':d','color','black', 'MarkerFaceColor', 'black'); hold on;" << endline;
		ss << "worst_plot	= plot( generation, worst_fitness,':o','color','black', 'MarkerFaceColor', 'black'); hold on;" << endline;
		ss << "elite_plot	= plot( generation, elite_fitness,'-*','color','black', 'MarkerFaceColor', 'black'); hold off;" << endline;
		ss << "legend([best_plot,avg_plot,worst_plot,elite_plot],'Best','Average','Worst','Elite',1);" << endline;
		ss << "h = xlabel('Generations'), ylabel('Fitness'), title('');" << endline;
		ss << "saveas(h,'" << base_filename << "_bin" << num_generations << ".eps','eps');" << endline;
		ss << "" << endline;

		output_buffer( ss.str(), config->matlab_filename );
	}
}

void DataCollector::log_latex_figures( int *bin_info )
{
	int bin_size = config->number_of_generations / config->number_of_bins;
	stringstream ss;

	ss << "\\begin{figure} [H] \n";
	ss << "\\centering\n";

	int start_index = ( bin_info[0] - bin_size + 1 );
	for( int i = 0; i < config->number_of_bins * 2; i += 2 )
	{
		if( bin_info[i+1] )
		{
			ss << "\\subfigure[";
			if( config->number_of_bins <= 2 )
				ss << ( config->with_guided_mutation ? "With GM -- " : "Without GM -- " );
			ss << "Bin ";
			ss << start_index << "-" << bin_info[i] << " (" << bin_info[i+1] << ") ]{\n"; 
			ss << "\\includegraphics[scale=.25]{" <<  base_filename << "_bin" << bin_info[i] << ".eps} % Number in bin: " << bin_info[i+1];
			ss << "\n}\n";
			start_index += bin_size;
		}
	}
	ss << "\\caption{" << config->experiment_name;
	ss << "}\\label{fig:" << config->filename;	
	ss /* << ( config->number_of_bins > 2 && config->with_guided_mutation ? "_gm" : "" ) */<< "}\n";
	ss << "\\end{figure}\n\n";

	output_buffer( ss.str(), config->latex_figures_filename );
}


void DataCollector::log_summary_table( double mean, double best, double worst, double std_dev, int failures )
{
	//	    \bf Method             & \bf Mean     & \bf Best      & \bf Worst     & \bf SD     & \bf Failures\\
	//   CGP.PBIL.BW.GM          & 1691          &    46              &  4461          &   984       &  0 \\

	stringstream ss;
	ss << config->experiment_name << "\t&";
	if( failures == config->number_of_runs )
		ss << "-\t&-\t&-\t&-\t&";
	else
		ss << mean << "\t& " << best << "\t& " << worst << "\t& " << (int)std_dev;
	if( failures != -1 )
		ss << "\t& " << failures;
	ss << " \\\\" << endline;
	if( failures != -1 )
		output_buffer( ss.str(), config->summary_table_filename );
	else
		output_buffer( ss.str(), config->success_rate_table_filename );
}

void DataCollector::log_bins()
{
	// AVERAGE PLOTS (in bins)

	int bin_size = config->number_of_generations / config->number_of_bins;
	if( config->number_of_generations % config->number_of_bins != 0 )
		debug << "WARNING: Number of bins is not a divisor of number of generations." << endline;

	int *bin_info = new int[config->number_of_bins * 2];

	for( int bin = 0; bin < config->number_of_bins; bin++ )
	{
		int number_of_generations = ( bin + 1 ) * bin_size;

		// count number of runs in the bin:
		int num_runs_in_bin = 0;
		for( int r = 0; r < config->number_of_runs; r++ )
		{
			bool has_evals_in_range		= generation_info[r][number_of_generations - bin_size].generation_number	!= -1;
			bool has_finished_in_range	=	number_of_generations == config->number_of_generations
										||	generation_info[r][number_of_generations].generation_number				== -1;

			if( has_evals_in_range && has_finished_in_range )
				num_runs_in_bin++;
		}

		generation_data *avg = new generation_data[config->number_of_generations];
		for( int g = 0; g < number_of_generations; g++ )
		{
			int number_of_runs = 0;
			avg[g].generation_number = g+1;
			avg[g].total_fitness = 0;
			avg[g].highest_fitness = 0;
			avg[g].lowest_fitness = 0;
			avg[g].elite_fitness = 0;
			avg[g].average_fitness = 0;
			avg[g].number_of_evals = 0;
			avg[g].number_of_runs = 0;
			for( int r = 0; r < config->number_of_runs; r++ )
			{
				bool has_evals_in_range		= generation_info[r][number_of_generations - bin_size].generation_number	!= -1;
				bool has_finished_in_range	=	number_of_generations == config->number_of_generations
											||	generation_info[r][number_of_generations].generation_number				== -1;

				if( has_evals_in_range && has_finished_in_range )
				{
					if( g > 0 && generation_info[r][g-1].elite_fitness == config->fitness_target )
						generation_info[r][g].elite_fitness = config->fitness_target;
					avg[g].elite_fitness += generation_info[r][g].elite_fitness;
					number_of_runs++;
					if( generation_info[r][g].total_fitness != 0 )
					{
						avg[g].total_fitness	+= generation_info[r][g].total_fitness;
						avg[g].highest_fitness	+= generation_info[r][g].highest_fitness;
						avg[g].lowest_fitness	+= generation_info[r][g].lowest_fitness;
						avg[g].average_fitness	+= generation_info[r][g].average_fitness;
						avg[g].number_of_evals	+= generation_info[r][g].number_of_evals;
						avg[g].number_of_runs++;
					}
				}
			}
			if( avg[g].number_of_runs > 0 )
			{
				avg[g].total_fitness	/= avg[g].number_of_runs;
				avg[g].highest_fitness	/= avg[g].number_of_runs;
				avg[g].lowest_fitness	/= avg[g].number_of_runs;			
				avg[g].average_fitness	/= avg[g].number_of_runs;
				avg[g].number_of_evals	/= avg[g].number_of_runs;
			}
			if( number_of_runs > 0 )
				avg[g].elite_fitness	/= number_of_runs;
		}

		bin_info[bin*2] = number_of_generations;
		bin_info[bin*2+1] = num_runs_in_bin;
		create_matlab_plot( avg, number_of_generations, num_runs_in_bin );
		delete [] avg;
	}
	
	log_latex_figures( bin_info );
	delete [] bin_info;
}


void DataCollector::log_experiment()
{
	// BIN GRAPHS
	log_bins();

	// SUCCESS RATE
	short success = 0;
	for( int i = 0; i < config->number_of_runs; i++ )
		if( run_info[i].elite_fitness == config->fitness_target )
			success++;

	experiment_info.success_rate = ( (double)success / (double)config->number_of_runs );
	experiment_info.total_evals /= config->number_of_runs;


    // CALCULATE STATISTICS
    int total_fitness = 0, total_evals = 0;
	int best_evals  = config->number_of_generations * config->population_size + 1;
    int worst_evals = -1;
    for( int k = 0; k < config->number_of_runs; k++ )
    {
		if( run_info[k].elite_fitness == config->fitness_target )
		{
			total_fitness += run_info[k].elite_fitness;
			total_evals += run_info[k].number_of_evals;
			if( run_info[k].number_of_evals <= best_evals )
				best_evals = run_info[k].number_of_evals;
			if( run_info[k].number_of_evals >= worst_evals )
				worst_evals = run_info[k].number_of_evals;
		}
    }
	main.precision( 10 );
	if( success > 0 )
		main    << "Avg Fitness = "     << total_fitness / success
			    << ", Avg Evals = "     << total_evals / success
				<< ", Best Evals = "    << best_evals
				<< ", Worst Evals = "   << worst_evals;
	else
		main	<< "All Runs Failed";
    // CALCULATE STANDARD DEVIATION
    int mean_evals = (double) total_evals / (double) success + .5;
    double std_dev = 0.;

    for( int k = 0; k < config->number_of_runs; k++ )
    {
		if( run_info[k].elite_fitness == config->fitness_target )
			std_dev +=      ( run_info[k].number_of_evals - mean_evals )
				        *   ( run_info[k].number_of_evals - mean_evals );
    }
	if( success > 0 )
		std_dev /= success;
    std_dev = sqrt( std_dev );
	main.width( 10 );
	main.precision( 10 );
    main << ", Std Dev = " << std_dev << endline;


    main.precision(2);
    main << "Success Rate=" << fixed << experiment_info.success_rate << "\t Total Evals=" << fixed << total_evals << endl << flush;

	log_summary_table( mean_evals, best_evals, worst_evals, std_dev, config->number_of_runs - success );
}

void DataCollector::log_run( int run_num, int number_of_generations, Model& model )
{
    run_info[run_num].elite_fitness = (int)model.elite_fitness;
	run_info[run_num].number_of_generations = number_of_generations;
	run_info[run_num].number_of_evals = model.total_evals;
	experiment_info.total_evals += model.total_evals;
    main    << "run = " << run_num
            << " elite fitness = " << run_info[run_num].elite_fitness            
            << ", generation = " << run_info[run_num].number_of_generations << endline;
	model.get_elite()->print( main );
}

void DataCollector::log_random_fitness( Model& model )
{
    int f;
    switch( config->fitness_test )
    {
    case POLE_BALANCING:
        f = pole_balancing_fitness( model.get_elite(), run, false );
        break;
    case XOR:
        f = fitnessXOR( model.get_elite(), run, false );
        break;
    case SIX_BIT:
        f = fitness6BitMux( model.get_elite(), run, false );
        break;
	case RETINA:
	case RETINA_SWITCHING:
		f = fitnessRetina( model.get_elite(), run, false );
		break;
    default:
        debug << "fitness_test ill defined: " << config->fitness_test << endline;
    }
    run << "Random fitness test = " << f << endl;
}


double DataCollector::get_success_rate()
{
	return experiment_info.success_rate;
}

void DataCollector::suppress()
{
	suppress_output = true;
}

#endif
