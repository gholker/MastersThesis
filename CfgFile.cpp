
#ifndef CFGFILE
#define CFGFILE

#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <cmath>

#include "CfgFile.hpp"
#include "DataCollector.hpp"


void CfgFile::print( std::ostream& fout )
{
    fout << this->to_string();
}

string CfgFile::to_string()
{
	stringstream ss;
	// GENERIC
	ss << "filename =				" << filename << endline;
	ss << "experiment_name =		" << experiment_name << endline;
	ss << "matlab_filename =		" << matlab_filename << endline;
	ss << "summary_table_filename =	" << summary_table_filename << endline;	
	ss << "latex_figures_filename = " << latex_figures_filename << endline;
	ss << "output_run =				" << output_run << endline;
	ss << "output_csv =				" << output_csv << endline;
	ss << "number_of_runs =			" << number_of_runs << endline;
	ss << "population_size =		" << population_size << endline;
	ss << "number_of_generations =  " << number_of_generations << endline;
	ss << "seed =					" << seed << endline;
	if( seed == 0 ) ss << "WARNING: seed is zero." << endline;
	ss << "pole_seed =				" << pole_seed << endline;
	if( pole_seed == 0 && fitness_test == POLE_BALANCING ) ss << "WARNING: pole_seed is zero." << endline;
    ss << "number_of_inputs =		" << number_of_inputs << endline;
    ss << "number_of_outputs =		" << number_of_outputs << endline;
	ss << "number_of_bins =			" << number_of_bins << endline;

	// EXPERIMENT
	ss << "stop_on_target_reached =	" << stop_on_target_reached << endline;
	ss << "number_of_experiments = " << number_of_experiments << endline;
	ss << "success_rate_table_filename = " << success_rate_table_filename << endline;

	// FITNESS
	ss << "fitness_test = "           << fitness_test;
	switch( fitness_test )
	{
	case XOR:
		ss << " XOR";
		break;
	case SIX_BIT:
		ss << " 6-MUX";
		break;
	case POLE_BALANCING:
		ss << " POLE_BALANCING";
		break;
	case RETINA:
		ss << " RETINA";
		break;
	case RETINA_SWITCHING:
		ss << " RETINA_SWITCHING" << endline;
		ss << "retina_switch_after = " << retina_switch_after;
	}
	ss << endline;

	ss << "fitness_target = "         << fitness_target << endline;
	ss << "fitness_direction = "	  << fitness_direction << " ";
	switch( fitness_direction )
	{
	case FITNESS_UP:
		ss << "FITNESS_UP";
		break;
	case FITNESS_DOWN:
		ss << "FITNESS DOWN";
		break;
	}
	ss << endline;

    // NEURAL NETWORK
    ss << "activation_function = "   << activation_function << " ";
    switch( activation_function )
    {
    case SIGMOID:
        ss << "SIGMOID";
        break;
    case BINARY:
        ss << "BINARY";
        break;
    }
    ss << endline;
	ss << "activation_slope = "		<< activation_slope << endline;
	if( activation_slope == 0. && activation_function != BINARY )
	{
		ss << "WARNING: activation_slope not set! Defaulting to 4.924273" << endline;
		activation_slope = 4.924273;
	}

	// GUIDED MUTATION
    ss << "with_guided_mutation = "   << with_guided_mutation << " ";
    if( with_guided_mutation == YES )
        ss << "YES";
    else
        ss << "NO";
    ss << endline;
	ss << "gm_on_best = " << ( gm_on_best ? "YES" : "NO" ) << endline;

	if( with_guided_mutation )
	{
        ss << "gm_prob_of_mutation = "    << gm_prob_of_mutation << endline;
        ss << "gm_population_percent = "  << gm_population_percent << endline;
	}

    // WEIGHT LEARNING
    ss << "weight_representation = "        << weight_representation << " ";
    switch( weight_representation )
    {
    case STOCHASTIC_HILL_CLIMBING:
        ss << "STOCHASTIC_HILL_CLIMBING" << endline;
		ss << "shc_mean_move = "          << shc_mean_move << endline;
        ss << "shc_std_dev_reduce = "     << shc_std_dev_reduce << endline;
		if( weight_learning != STOCHASTIC_HILL_CLIMBING )
		{
			DataCollector::debug << "WARNING: weight_learning is not SHC and weight_representation is. Changing weight_learning to SHC." << endline;
			weight_learning = STOCHASTIC_HILL_CLIMBING;
				
		}
        break;
	case BITWISE:
		ss << "BITWISE" << endline;
		break;
    }
	
	ss << "weight_learning = "        << weight_learning << " ";
	switch( weight_learning )
	{
	case PBIL:
		ss << "PBIL" << endline;
		break;
	case PIPE:
		ss << "PIPE" << endline;
		break;
	case STOCHASTIC_HILL_CLIMBING:
		ss << "SHC" << endline;
	}


	ss << endline;

    ss << "structural_learning = "    << structural_learning << " ";
	if( structural_learning == PIPE || weight_learning == PIPE )
	{
        ss << "PIPE" << endline;
        ss << "pipe_prob_elitist_learn = "<< pipe_prob_elitist_learn << endline;
        ss << "pipe_learning_rate = "     << pipe_learning_rate << endline;
        ss << "pipe_fitness_constant = "  << pipe_fitness_constant << endline;
        ss << "pipe_prob_mutation = "     << pipe_prob_mutation << endline;
        ss << "pipe_mutation_rate = "     << pipe_mutation_rate << endline;
	}
	else if( structural_learning == PBIL || weight_learning == PBIL )
	{
		ss << "PBIL" << endline;
		ss << "pbil_learning_rate = "		<< pbil_learning_rate << endline;
		ss << "pbil_mutation_probability = "<< pbil_mutation_probability << endline;
		ss << "pbil_mutation_shift = "		<< pbil_mutation_shift << endline;
	}

    ss << "representation = "         << representation << " ";
    switch( representation )
    {
    case GEP:
        ss << "GEP" << endline;
        ss << "gep_min_arity = "            << gep_min_arity << endline;
        ss << "gep_max_arity = "            << gep_max_arity << endline;
        ss << "gep_head_length = "          << gep_head_length << endline;
        ss << "gep_num_genes = "            << gep_num_genes << endline;
        ss << "gep_num_weights = "          << gep_num_weights << endline;
        break;
    case CGP:
        ss << "CGP" << endline;
        ss << "cgp_arity = "                << cgp_arity << endline;
        ss << "cgp_number_of_functions = "  << cgp_number_of_functions << endline;
        for( int i = 0; i < cgp_number_of_functions; i++ )
            ss << "cgp_functions[" << i << "] = "   << cgp_functions[i] << endline;
        ss << "cgp_number_of_rows = "       << cgp_number_of_rows << endline;
        ss << "cgp_number_of_columns = "    << cgp_number_of_columns << endline;
        ss << "cgp_levels_back = "          << cgp_levels_back << endline;
		ss << "cgp_num_weights = "			<< cgp_num_weights << endline;
		ss << "cgp_weight_redirect = "		<< cgp_weight_redirect << endline;
        break;
    }
    ss << endline;

	ss << "bw_range = " << bw_range << endline;

    return ss.str();
}


void CfgFile::evaluate( char token[100], char value[100] )
{
    // GENERIC
	if( strcmp( token, "number_of_runs" ) == 0 )
		number_of_runs = atoi( value );
    else if( strcmp( token, "filename" ) == 0 )
        filename = value;
	else if( strcmp( token, "experiment_name" ) == 0 )
		experiment_name = value;
	else if( strcmp( token, "matlab_filename" ) == 0 )
		matlab_filename = value;
	else if( strcmp( token, "summary_table_filename" ) == 0 )
		summary_table_filename = value;
	else if( strcmp( token, "success_rate_table_filename" ) == 0 )
		success_rate_table_filename = value;
	else if( strcmp( token, "latex_figures_filename" ) == 0 )
		latex_figures_filename = value;
	else if( strcmp( token, "stop_on_target_reached" ) == 0 )
		if( strcmp( token, "YES" ) == 0 )
			stop_on_target_reached = YES;
		else
			stop_on_target_reached = NO;
	else if( strcmp( token, "population_size" ) == 0 )
		population_size = atoi( value );
	else if( strcmp( token, "number_of_generations" ) == 0 )
	{
		number_of_generations = atoi( value );
		shc_std_dev_reduce = pow( (double).001, (double)1. / number_of_generations );
	}
    else if( strcmp( token, "number_of_inputs" ) == 0 )
        number_of_inputs = atoi( value );
    else if( strcmp( token, "number_of_outputs" ) == 0 )
        number_of_outputs = atoi( value );
	else if( strcmp( token, "number_of_bins" ) == 0 )
		number_of_bins = atoi( value );
	else if( strcmp( token, "seed" ) == 0 )
		seed  = atol( value );
	else if( strcmp( token, "pole_seed" ) == 0 )
		pole_seed  = atol( value );
    else if( strcmp( token, "representation" ) == 0 )
        if( strcmp( value, "GEP" ) == 0 )
            representation = GEP;
        else if( strcmp( value, "CGP" ) == 0 )
            representation = CGP;
        else
            DataCollector::debug << "For representation, invalid value: " << value << endline;
	else if( strcmp( token, "weight_representation" ) == 0 )
		if( strcmp( value, "BITWISE" ) == 0 )
			weight_representation = BITWISE;
		else
			weight_representation = STOCHASTIC_HILL_CLIMBING;
    else if( strcmp( token, "structural_learning" ) == 0 )
        if( strcmp( value, "PIPE" ) == 0 )
            structural_learning = PIPE;
		else if( strcmp( value, "PBIL" ) == 0 )
			structural_learning = PBIL;
        else
            DataCollector::debug << "For structural_learning, invalid value: " << value << endline;	
    else if( strcmp( token, "weight_learning" ) == 0 )
        if( strcmp( value, "STOCHASTIC_HILL_CLIMBING" ) == 0 )
            weight_learning = STOCHASTIC_HILL_CLIMBING;
        else if( strcmp( value, "PIPE" ) == 0 )
            weight_learning = PIPE;
		else if( strcmp( value, "PBIL" ) == 0 )
            weight_learning = PBIL;
        else
            DataCollector::debug << "For weight_learning, invalid value: " << value << endline;
    else if( strcmp( token, "with_guided_mutation" ) == 0 )
        if( strcmp( value, "YES" ) == 0 )
            with_guided_mutation = YES;
        else
            with_guided_mutation = NO;
	else if( strcmp( token, "output_run" ) == 0 )
		if( strcmp( value, "YES" ) == 0 )
			output_run = YES;
		else
			output_run = NO;
	else if( strcmp( token, "output_csv" ) == 0 )
		if( strcmp( value, "YES" ) == 0 )
			output_csv = YES;
		else
			output_csv = NO;
	else if( strcmp( token, "number_of_experiments" ) == 0 )
		number_of_experiments = atoi( value );

    // FITNESS
	else if( strcmp( token, "fitness_test" ) == 0 )
	{
		if( strcmp( value, "POLE_BALANCING" ) == 0 )
		{
			fitness_test = POLE_BALANCING;
			fitness_target = 15000;
			fitness_direction = FITNESS_UP;
			number_of_inputs = 5;
			number_of_outputs = 2;
			activation_function = SIGMOID;
			activation_slope = 4.924273;
		}
        else if( strcmp( value, "DOUBLE_POLE_BALANCING" ) == 0 )
            fitness_test = DOUBLE_POLE_BALANCING;
		else if( strcmp( value, "SIX_BIT" ) == 0 )
		{
			fitness_test = SIX_BIT;
			fitness_target = 64;
			fitness_direction = FITNESS_UP;
			number_of_inputs = 7;
			number_of_outputs = 1;
			activation_function = BINARY;
		}
		else if( strcmp( value, "XOR" ) == 0 )
		{
			fitness_test = XOR;
			fitness_target = 4;
			fitness_direction = FITNESS_UP;
			number_of_inputs = 3;
			number_of_outputs = 1;
			activation_function = BINARY;
		}
		else if( strcmp( value, "RETINA" ) == 0 || strcmp( value, "RETINA_SWITCHING" ) == 0)
		{
			fitness_test = RETINA;
			fitness_target = 256;
			fitness_direction = FITNESS_UP;
			number_of_inputs = 9;
			number_of_outputs = 1;
			activation_function = BINARY;
		}
		
        else
            DataCollector::debug << "For token: " << token << ", value not found: " << value << endline;

		if( strcmp( value, "RETINA_SWITCHING" ) == 0 ) // OTHER SETTINGS ARE ABOVE WITH RETINA
		{
			fitness_test = RETINA_SWITCHING;
			retina_switch_after = 20;
		}
	}
	else if( strcmp( token, "retina_switch_after" ) == 0 )
		retina_switch_after = atoi( value );
    else if( strcmp( token, "fitness_target" ) == 0 )
        fitness_target = atoi( value );
    else if( strcmp( token, "fitness_direction" ) == 0 )
    {
        if( strcmp( value, "FITNESS_UP" ) == 0 )
            fitness_direction = FITNESS_UP;
        else if( strcmp( value, "FITNESS_DOWN" ) == 0 )
            fitness_direction = FITNESS_DOWN;
    }

    // GUIDED MUTATION
	else if( strcmp( token, "gm_prob_of_mutation" ) == 0 )
		gm_prob_of_mutation = atof( value );
	else if( strcmp( token, "gm_population_percent" ) == 0 )
		gm_population_percent  = atof( value );
	else if( strcmp( token, "gm_on_best" ) == 0 )
		if( strcmp( value, "YES" ) == 0 )
			gm_on_best = YES;
		else
			gm_on_best = NO;

    // GEP
	else if( strcmp( token, "gep_min_arity" ) == 0 )
		gep_min_arity = atoi( value );
	else if( strcmp( token, "gep_max_arity" ) == 0 )
		gep_max_arity = atoi( value );
	else if( strcmp( token, "gep_head_length" ) == 0 )
		gep_head_length = atoi( value );
	else if( strcmp( token, "gep_num_genes" ) == 0 )
		gep_num_genes = atoi( value );
	else if( strcmp( token, "gep_num_weights" ) == 0 )
		gep_num_weights = atoi( value );

    // NEURAL NETWORK
    else if( strcmp( token, "activation_function" ) == 0 )
        if( strcmp( value, "SIGMOID" ) == 0 )
            activation_function = SIGMOID;
        else if( strcmp( value, "BINARY" ) == 0 )
            activation_function = BINARY;
		else if( strcmp( value, "HYPERBOLIC_TANGENT" ) == 0 )
			activation_function = HYPERBOLIC_TANGENT;
        else
            DataCollector::debug << "For activation_function, invalid value: " << value << endline;
	else if( strcmp( token, "activation_slope" ) == 0 )
		activation_slope = atof( value );

    // STOCHASTIC HILL CLIMBING
	else if( strcmp( token, "shc_mean_move" ) == 0 )
		shc_mean_move = atof( value );
	else if( strcmp( token, "shc_std_dev_reduce" ) == 0 )
	{
		if( shc_std_dev_reduce != 0. )
			DataCollector::debug << "WARNING: over-writing previous value of shc_std_dev_reduce (" << shc_std_dev_reduce << ")" << endline;
		shc_std_dev_reduce = atof( value );
	}

    // PIPE
    else if( strcmp( token, "pipe_prob_elitist_learn" ) == 0 )
        pipe_prob_elitist_learn = atof( value );
    else if( strcmp( token, "pipe_learning_rate" ) == 0 )
        pipe_learning_rate = atof( value );
    else if( strcmp( token, "pipe_fitness_constant" ) == 0 )
        pipe_fitness_constant = atof( value );
    else if( strcmp( token, "pipe_prob_mutation" ) == 0 )
        pipe_prob_mutation = atof( value );
    else if( strcmp( token, "pipe_mutation_rate" ) == 0 )
        pipe_mutation_rate = atof( value );

	// PBIL
	else if( strcmp( token, "pbil_learning_rate" ) == 0 )
        pbil_learning_rate = atof( value );
	else if( strcmp( token, "pbil_mutation_probability" ) == 0 )
        pbil_mutation_probability = atof( value );
	else if( strcmp( token, "pbil_mutation_shift" ) == 0 )
        pbil_mutation_shift = atof( value );

    // CGP
    else if( strcmp( token, "cgp_arity" ) == 0 )
        cgp_arity = atoi( value );
    else if( strcmp( token, "cgp_number_of_functions" ) == 0 )
    {
        cgp_number_of_functions = atoi( value );
		if( cgp_functions )
		{
			delete [] cgp_functions;
			cgp_number_of_parsed_functions = 0;
		}
        cgp_functions = new short[cgp_number_of_functions];
        if( activation_function )
            cgp_functions[0] = activation_function;
        else
            cgp_functions[0] = SIGMOID;
    }
	else if( strcmp( token, "cgp_function" ) == 0 )
		if( strcmp( value, "BINARY" ) == 0 )
			cgp_functions[cgp_number_of_parsed_functions++] = BINARY;
		else if( strcmp( value, "SIGMOID" ) == 0 )
			cgp_functions[cgp_number_of_parsed_functions++] = SIGMOID;
		else if( strcmp( value, "HYPERBOLIC_TANGENT" ) == 0 )
			cgp_functions[cgp_number_of_parsed_functions++] = HYPERBOLIC_TANGENT;
		else
			DataCollector::debug << "Invalid value for cgp_function: " << value << endline;
    else if( strcmp( token, "cgp_number_of_rows" ) == 0 )
        cgp_number_of_rows = atoi( value );
    else if( strcmp( token, "cgp_number_of_columns" ) == 0 )
        cgp_number_of_columns = atoi( value );
    else if( strcmp( token, "cgp_levels_back" ) == 0 )
        cgp_levels_back = atoi( value );
	else if( strcmp( token, "cgp_num_weights" ) == 0 )
		cgp_num_weights = atoi( value );
	else if( strcmp( token, "cgp_weight_redirect" ) == 0 )
		if( strcmp( value, "YES" ) == 0 )
			cgp_weight_redirect = YES;
		else
			cgp_weight_redirect = NO;
	else if( strcmp( token, "bw_range" ) == 0 )
		bw_range = atof( value );
    else
        DataCollector::debug << "Token not found: " << token << ", with value: " << value << endline;
}

bool CfgFile::default_exists()
{
	using namespace std;
	ifstream fin;
	fin.open( default_filename.c_str() );
	bool return_value = !fin.fail();
	fin.close();
	return return_value;
}

void CfgFile::readFile( const char *file_name )
{
	using namespace std;
	ifstream fin;
	char ch;
	char token[100], value[100];
	bool in_comment = false, in_token = true, in_string = false;
	int token_count = 0, value_count = 0;
	token[0] = value[0] = '\0';

	fin.open( file_name );
	if( fin.fail() )
	{
		DataCollector::debug << "File \"" << file_name << "\" failed to open." << endline;
		exit( 1 );
	}
	else
	{
		while( fin.get( ch ) )
		{
			switch( ch )
			{
			case '"':
				if( !in_string )
					in_string = true;
				else
					in_string = false;
				break;
            case '\t':
			case ' ':
				if( in_string )
					goto addchar;
				if( in_token && token_count > 0 )
					in_token = false;
				break;
			case '#':
			case '/':
				in_comment = true;
				break;
			case 0x0D:
				break;
			case 0x0A:
addtoken:
				token[token_count] = '\0';
				value[value_count] = '\0';
				//cout << "token=" << token << ", value=" << value << endl;
				if(	!(	token[0] == '\0'
					||	value[0] == '\0' ))
				{
					evaluate( token, value );
				}
				token[0] = value[0] = '\0';
				token_count = value_count = 0;
				in_comment = false;
				in_token = true;
				break;
addchar:
			default:
				if( in_token && !in_comment )
				{
					token[token_count] = ch;
					token_count++;
				}
				else if( !in_comment )
				{
					value[value_count] = ch;
					value_count++;
				}
				break;
			}
		}
		if( token_count > 0 && value_count > 0 )
			goto addtoken;
		if( cgp_number_of_functions != cgp_number_of_parsed_functions )
			DataCollector::debug	<< "CGP: #ofFunctions not equal to the #ofParsedFunctions. "
									<< cgp_number_of_functions << " != " << cgp_number_of_parsed_functions << endline;
	}
	fin.close();
}

CfgFile::CfgFile( char *file_name )
{
    set_defaults();
	if( default_exists() )
		readFile( default_filename.c_str() );
	readFile( file_name );
}

CfgFile::~CfgFile()
{
	delete [] cgp_functions;
}

bool CfgFile::is_PIPE_GEP()
{
    return  representation         == GEP
        &&  structural_learning    == PIPE;
}

void CfgFile::set_defaults()
{
	default_filename = "default.cfg";
	matlab_filename = "default.m";
	summary_table_filename = "summary_table.txt";
	latex_figures_filename = "latex_figures.tex";
	experiment_name	= "experiment name";
	success_rate_table_filename = "success_rate_table.txt";

	// EXPERIMENT
	stop_on_target_reached = NO;
	number_of_experiments = 1;

    // FITNESS
	fitness_test = 0;
    fitness_direction = 0;
    fitness_target = 0;
	retina_switch_after = 20;

    // GUIDED MUTATION
	gm_prob_of_mutation = 0;
	gm_population_percent = 0;
	gm_on_best = NO;

	// GEP
	gep_min_arity = 0;
	gep_max_arity = 0;
	gep_head_length = 0;
	gep_num_genes = 0;
	gep_num_weights = 0;

	// GENERIC
	output_csv = 0;
	output_run = 0;
	population_size = 0;
	number_of_inputs = 0;
	number_of_outputs = 0;
	number_of_generations = 0;
	number_of_runs = 0;
	seed = -1;
	pole_seed = -1;
	representation = 0;
	weight_representation = STOCHASTIC_HILL_CLIMBING;
	structural_learning = 0;
	weight_learning = 0;
	with_guided_mutation = 0;
    number_of_weight_arrays = 0;
    number_of_weights = 0;
	number_of_bins = 1;

	// NEURAL NETWORK
	activation_function = 0;
	activation_slope = 0;

    // STOCHASTIC HILL CLIMBING
	shc_std_dev_reduce = 0;
	shc_mean_move = 0;

	// PIPE
	pipe_prob_elitist_learn = 0;
	pipe_learning_rate = 0;
	pipe_fitness_constant = 0;
	pipe_prob_mutation = 0;
	pipe_mutation_rate = 0;

	// PBIL
	pbil_learning_rate = 0;
	pbil_mutation_probability = 0;
	pbil_mutation_shift = 0;

	// BITWISE
	bw_range = 2.0;

	// CGP
    cgp_arity = 0;
    cgp_number_of_functions = 0;
	cgp_number_of_parsed_functions = 0;
    cgp_functions = NULL;
    cgp_number_of_rows = 0;
    cgp_number_of_columns = 0;
    cgp_levels_back = 0;
	cgp_num_weights = 0;
	cgp_weight_redirect = NO;
}

int CfgFile::get_cgp_length()
{
	return cgp_number_of_rows * cgp_number_of_columns * CGPModelNode::length() + number_of_outputs;
}

int CfgFile::get_cgp_num_weights()
{
	if( cgp_weight_redirect == YES )
		return cgp_num_weights;
	else
	{
		int num_weights = 0;
		for( int column = 0; column < cgp_number_of_columns; column++ )
		{
			int number_of_possibilities = column * cgp_number_of_rows;
			if( number_of_possibilities > cgp_levels_back * cgp_number_of_rows )
				number_of_possibilities = cgp_levels_back * cgp_number_of_rows;

			num_weights += cgp_number_of_rows * ( number_of_inputs + number_of_possibilities );
		}
		return num_weights;
	}
}

int CfgFile::get_cgp_number_of_possibilities( int column )
{
	return column * cgp_number_of_rows - get_cgp_number_of_inaccessible_nodes( column );
}

int CfgFile::get_cgp_number_of_inaccessible_nodes( int column )
{
	int num = column * cgp_number_of_rows - cgp_levels_back * cgp_number_of_rows;
    return ( num > 0 ) ? num : 0;
}

int CfgFile::get_cgp_weight_index( int in, int out )
{
	int weight_index = 0;
	for( int column = 0; column < cgp_number_of_columns; column++ )
    {
		for( int row = 0; row < cgp_number_of_rows; row++ )
		{
			int number_of_possibilities = get_cgp_number_of_possibilities( column );

			int modifier = 0;
			if( in >= number_of_inputs )
			{
				int x = column - cgp_levels_back;
				modifier = -1 * ( x > 0 ? x : 0 ) * cgp_number_of_rows;
			}
			if( column * cgp_number_of_rows + row == out - number_of_inputs )
				return weight_index + in + modifier;
			else
				weight_index += number_of_inputs + number_of_possibilities;
		}
    }
	return weight_index;
}

#endif

