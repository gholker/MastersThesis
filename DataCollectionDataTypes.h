#ifndef DATA_COLLECTION_DATA_TYPES
#define DATA_COLLECTION_DATA_TYPES

typedef struct
{
    double success_rate;
    int total_evals;
    float average_fitness;
    int best_evals;
    int worst_evals;
    float average_evals;
    float standard_deviation_evals;
} experiment_data;

typedef struct
{
    int run_number;
    int number_of_generations;
	int elite_fitness;
	int elite_size;
	int number_of_evals;
} run_data; 

typedef struct
{
    int generation_number;
    int total_fitness;
    int highest_fitness;
    int lowest_fitness;
    int elite_fitness;
    int average_fitness;
    int number_of_evals;
	int number_of_runs;
} generation_data;

#endif
