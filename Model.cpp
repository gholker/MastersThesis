
#ifndef MODEL
#define MODEL

#include <iostream>
#include "Model.hpp"
#include "GEPNet.hpp"
#include "PoleBalancing.hpp"
#include "FitnessFunctions.hpp"

CfgFile* Model::config = NULL;

void Model::set_config( CfgFile& cfg )
{
    config = &cfg;
}

Neuron** Model::get_inputs()
{
    return inputs;
}

void Model::set_data_collector( DataCollector& dc )
{
    this->dc = &dc;
}

Model::Model()
{
	is_elitist_learning = false;
    if( config == NULL )
        DataCollector::debug << "Model::config is NULL." << endline;
    init();
}

void Model::init()
{
    population = NULL;
	ppc_array = NULL;
	inputs = NULL;
	shc_weight_array = NULL;
	bitwise_weights = NULL;

    // FITNESS
    total_evals = 0;
    if( config->fitness_direction == FITNESS_UP )
        elite_fitness = 0;
    else
        elite_fitness = MAX_FITNESS;
    elite_individual = NULL;
	best_individual = NULL;

	// STRUCTURAL LEARNING MODEL
    switch( config->representation )
    {
	case GEP:
        ppc_array = new PrototypeChromosome*[config->number_of_outputs];
        for( int i = 0; i < config->number_of_outputs; i++ )
            ppc_array[i] = new PrototypeChromosome();

		// INPUTS
		inputs = new Neuron*[ config->number_of_inputs ];
		for( int i = 0; i < config->number_of_inputs; i++ )
			inputs[i] = new Neuron();

        for( int i = 0; i < config->number_of_outputs; i++ )
        {
            ppc_array[i]->init();
        }

		// WEIGHT NUMBER
		config->number_of_weight_arrays = config->number_of_outputs;
		config->number_of_weights = config->gep_num_weights;
		break;
	case CGP:
		// WEIGHT NUMBER
		config->number_of_weight_arrays = 1;
		config->number_of_weights = config->get_cgp_num_weights();
		break;
	default:
        DataCollector::debug << "The representation model does not exist." << endline;
		break;
    }

    // WEIGHT LEARNING MODEL
    switch( config->weight_representation )
    {
	case STOCHASTIC_HILL_CLIMBING:
        shc_weight_array = new SHCWeightCollection*[config->number_of_weight_arrays];
        for( int i = 0; i < config->number_of_weight_arrays; i++ )
            shc_weight_array[i] = new SHCWeightCollection(
                                    config->number_of_weights, 2., 0., 1.,
                                    config->shc_mean_move, config->shc_std_dev_reduce );
		break;
	case BITWISE:
        bitwise_weights = new BitwiseWeights[config->number_of_weight_arrays];
        for( int i = 0; i < config->number_of_weight_arrays; i++ )
            bitwise_weights[i].init( config->number_of_weights );
		break;
	default:
        DataCollector::debug << "The weight model does not exist." << endline;
		break;
    }
}

Model::~Model()
{
	if( elite_individual )
		delete elite_individual;
	if( best_individual && best_individual != elite_individual )
		delete best_individual;

    delete_population();

	if( inputs )
	{
		for( int i = 0; i < config->number_of_inputs; i++ )
			delete inputs[i];
		delete [] inputs;
	}

	if( shc_weight_array )
	{
		for( int i = 0; i < config->number_of_weight_arrays; i++ )
			if( shc_weight_array[i] )
				delete shc_weight_array[i];
		delete [] shc_weight_array;
	}

	if( bitwise_weights )
        delete [] bitwise_weights;

	if( ppc_array )
	{
        for( int i = 0; i < config->number_of_outputs; i++ )
			if( ppc_array[i] )
				delete ppc_array[i];
        delete [] ppc_array;
    }

}


void Model::mutate( RandomNumberGenerator& rng )
{

	// REPRESENTATION
    switch( config->representation )
    {
	case GEP:
        for( int i = 0; i < config->number_of_outputs; i++ )
		{
			switch( config->structural_learning )
			{
			case PIPE:
				ppc_array[i]->pipe_mutate( elite_individual->get_gep_genes()[i], rng );
				break;
			case PBIL:
				ppc_array[i]->pbil_mutate( elite_individual->get_gep_genes()[i], rng );
				break;
			}
		}
		break;
	case CGP:
		switch( config->structural_learning )
		{
		case PIPE:
			cgp_model.pipe_mutate( rng );
			break;
		case PBIL:
			cgp_model.pbil_mutate( rng );
			break;
		}
		break;
    }

	// WEIGHTS
	if( config->weight_representation == BITWISE )
		for( int i = 0; i < config->number_of_weight_arrays; i++ )
		{
			switch( config->weight_learning )
			{
			case PIPE:
				bitwise_weights[i].pipe_mutate( rng );
				break;
			case PBIL:
				bitwise_weights[i].pbil_mutate( rng );
				break;
			}
		}
}

void Model::init_population()
{
    delete_population();
    population = new Individual*[config->population_size];
}

void Model::delete_population()
{
	if( population )
	{
		for( int i = 0; i < config->population_size; i++ )
			if( !( population[i] == elite_individual || population[i] == best_individual ) )
				delete population[i];
    	delete [] population;
    	population = NULL;
	}
}

Individual* Model::create_pipe_gep_individual( double **weight_arrays, bool with_gm, RandomNumberGenerator& rng )
{
    GepNN **temp_genes;
    GEPNet **nets;
    MIMO_FF_GEPNet *net;

    Individual *out = new Individual();

    temp_genes      = new GepNN*[config->number_of_outputs];
    nets            = new GEPNet*[config->number_of_outputs];

    // CREATE A GENE FROM THE PPC
    for( int g = 0; g < config->number_of_outputs; g++ )
    {
        temp_genes[g] = ppc_array[g]->get_genotype( rng );
		temp_genes[g]->setWeights( weight_arrays[g], config->gep_num_weights );

    }

    // GUIDED MUTATION
	if( with_gm )
	{
		for( int g = 0; g < config->number_of_outputs; g++ )
			ppc_array[g]->guided_mutation( get_gm_indiv()->get_gep_genes()[g], temp_genes[g], rng );
	}

    for( int g = 0; g < config->number_of_outputs; g++ )
        out->set_gep_gene( temp_genes[g], g );

    // CREATE A NETWORK FROM GENES
    for( int g = 0; g < config->number_of_outputs; g++ )
        nets[g] = temp_genes[g]->createNetwork( inputs );

    net = new MIMO_FF_GEPNet( *config, nets );

    out->set_gepnet( net );
    delete [] temp_genes;
    delete [] nets;

    return out;
}

void Model::sample( RandomNumberGenerator& rng )
{    
    is_elitist_learning = false;
    init_population();

    for( int j = 0; j < config->population_size; j++ )
    {
		// GUIDED MUTATION?
        bool use_guided_mutation =
                config->with_guided_mutation == YES
            &&  j >= ( config->population_size * ( 1. - config->gm_population_percent ) )
            &&	elite_individual != NULL;



		// WEIGHTS
		double **weight_arrays = new double*[config->number_of_weight_arrays];
		bitwise_weight_value **weight_value_arrays = new bitwise_weight_value*[config->number_of_weight_arrays];
		for( int i = 0; i < config->number_of_weight_arrays; i++ )
		{
			switch( config->weight_representation )
			{
			case STOCHASTIC_HILL_CLIMBING:
				weight_arrays[i] = shc_weight_array[i]->getWeights( rng );

                weight_value_arrays[i] = new bitwise_weight_value[config->number_of_weights];
                for( int j = 0; j < config->number_of_weights; j++ )
                    weight_value_arrays[i][j].decimal = weight_arrays[i][j];

				break;
			case BITWISE:
                {
                    bitwise_weight_value *values = bitwise_weights[i].get_weights( rng );
                    weight_value_arrays[i] = new bitwise_weight_value[config->number_of_weights];
                    for( int j = 0; j < config->number_of_weights; j++ )
                        weight_value_arrays[i][j] = values[j];

                    weight_arrays[i] = new double[config->number_of_weights];
                    for( int j = 0; j < config->number_of_weights; j++ )
                        weight_arrays[i][j] = weight_value_arrays[i][j].decimal;
                }
				break;
			}
		}		

		// WEIGHT GM
		if( use_guided_mutation )
		{
			for( int i = 0; i < config->number_of_weight_arrays; i++ )
			{
				for( int j = 0; j < config->number_of_weights; j++ )
				{
					switch( config->weight_representation )
					{
					case STOCHASTIC_HILL_CLIMBING:

						if( rng.getRandom() < config->gm_prob_of_mutation )
							weight_arrays[i][j] = get_gm_indiv()->get_weight_values( i )[j].decimal;

						break;
					case BITWISE:

						bitwise_weights[i].guided_mutation( get_gm_indiv()->get_weight_values( i ), weight_value_arrays[i], rng );

						break;
					}
				}
			}
		}

		// REPRESENTATION
        switch( config->representation )
        {
		case GEP:
            population[j] = create_pipe_gep_individual( weight_arrays, use_guided_mutation, rng );
			break;
		case CGP:
			{
				population[j] = new Individual();
				int *gene = cgp_model.get_genotype( rng );
				// GM
				if( use_guided_mutation )
					cgp_model.guided_mutation( get_gm_indiv()->get_cgp_array(), gene, rng );
				population[j]->cgp_graph_init( gene, weight_arrays[0] );
			}
			break;
		default:
            DataCollector::debug << "Model::sample, no model specified.\n";
        }



        for( int i = 0; i < config->number_of_weight_arrays; i++ )
		{
            population[j]->set_weight_values( i, weight_value_arrays[i] );
			population[j]->set_weights( i, weight_arrays[i] );
		}

		for( int i = 0; i < config->number_of_weight_arrays; i++ )
		{
		    if( weight_arrays[i] )
                delete [] weight_arrays[i];
            if( weight_value_arrays[i] )
                delete [] weight_value_arrays[i];
		}
		delete [] weight_arrays;
		delete [] weight_value_arrays;
    }
}

void Model::measure_fitness( ostream& fout )
{
    total_fitness = 0;
    highest_fitness = -1;
    lowest_fitness = MAX_FITNESS;
    bool elite_set_in_this_generation = false;

    for( int i = 0; i < config->population_size; i++ )
    {
        switch( config->fitness_test )
        {
        case POLE_BALANCING:
            for( int test = 0; test < 10; test++ )
                population[i]->fitness += pole_balancing_fitness( population[i], fout, false );
            break;
		case XOR:
			population[i]->fitness = fitnessXOR( population[i], fout, false );
			break;
		case SIX_BIT:
			population[i]->fitness = fitness6BitMux( population[i], fout, false );
			break;
		case RETINA:
		case RETINA_SWITCHING:
			population[i]->fitness = fitnessRetina( population[i], fout, false );
			break;
        default:
            DataCollector::debug << "fitness_test ill defined: " << config->fitness_test << endline;
        }
        total_fitness += population[i]->fitness;

        // LOWEST and HIGHEST FITNESS
        if( population[i]->fitness < lowest_fitness )
        {
            lowest_fitness = population[i]->fitness;
            lowest_fitness_index = i;
        }
        if( population[i]->fitness > highest_fitness )
        {
            highest_fitness = population[i]->fitness;
            highest_fitness_index = i;
        }

		if( !stop_condition_met() )
			total_evals++;

        // FIND ELITE
        if(     ( config->fitness_direction == FITNESS_UP && population[i]->fitness >= elite_fitness )
            ||  ( config->fitness_direction == FITNESS_DOWN && population[i]->fitness <= elite_fitness ) )
        {
                elite_fitness = population[i]->fitness;
                if( elite_individual && !elite_set_in_this_generation && elite_individual != best_individual )
                    delete elite_individual;
                elite_individual = population[i];
                elite_set_in_this_generation = true;
        }

        // CHECK FOR STOP CONDITION (no extra evaluations)
		if( stop_condition_met() && config->stop_on_target_reached )
            break;
    }
	if( best_individual && best_individual != elite_individual )
		delete best_individual;
	best_individual = NULL;
	
	if( config->fitness_direction == FITNESS_UP )
		best_individual = population[highest_fitness_index];
	else 
		best_individual = population[lowest_fitness_index];
}

bool Model::stop_condition_met()
{
	return		( elite_fitness >= config->fitness_target && config->fitness_direction == FITNESS_UP )
		    ||  ( elite_fitness <= config->fitness_target && config->fitness_direction == FITNESS_DOWN );
}

void Model::adapt( Individual *indiv )
{
    switch( config->representation )
    {
	case GEP:
		{
			GepNN **genes = indiv->get_gep_genes();
			for( int i = 0; i < config->number_of_outputs; i++ )
			{
				genes[i]->fitness = indiv->fitness;
				if( config->structural_learning == PIPE )
					ppc_array[i]->pipe_adapt( genes[i], highest_fitness, elite_fitness );
				else if( config->structural_learning == PBIL )
					ppc_array[i]->pbil_adapt( genes[i] );
			}
		}
		break;
	case CGP:
		if( config->structural_learning == PIPE )
			cgp_model.pipe_adapt( indiv->get_cgp_array(), highest_fitness, elite_fitness );
		else if( config->structural_learning == PBIL )
			cgp_model.pbil_adapt( indiv->get_cgp_array() );
		else
			DataCollector::debug << "Structural Learning illegal: " << config->structural_learning << endline;
		break;
    }

	for( int i = 0; i < config->number_of_weight_arrays; i++ )
	{
		switch( config->weight_representation )
		{
		case STOCHASTIC_HILL_CLIMBING:
			shc_weight_array[i]->setBestWeights( i, indiv );
			break;
		case BITWISE:
			if( config->weight_learning == PBIL )
				bitwise_weights[i].pbil_adapt( indiv->get_weight_values( i ) );
			else if( config->weight_learning == PIPE )
				bitwise_weights[i].pipe_adapt( indiv->get_weight_values( i ), highest_fitness, elite_fitness );
			else
				DataCollector::debug << "weight_learning not legal: " << config->weight_learning << endline;
			break;
		}
	}
}


Individual* Model::get_elite()
{
    return elite_individual;
}

Individual* Model::get_gm_indiv()
{
	if( config->gm_on_best )
		return get_best();
	else
		return get_elite();
}

Individual* Model::get_best()
{
	return best_individual;
}

void Model::print( ostream& fout )
{
	// STRUCTURE
    switch( config->representation )
    {
	case GEP:
        for( int i = 0; i < config->number_of_outputs; i++ )
            ppc_array[i]->print( fout );
		break;
	case CGP:
		cgp_model.print( fout );
		break;
    }

	// WEIGHTS
	for( int i = 0; i < config->number_of_weight_arrays; i++ )
	{
	    switch( config->weight_representation )
        {
        case STOCHASTIC_HILL_CLIMBING:
            shc_weight_array[i]->print( fout );
            break;
        case BITWISE:
            bitwise_weights[i].print( fout );
            break;
        }
	}
}

#endif

