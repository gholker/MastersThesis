
#ifndef PPC_H
#define PPC_H

#include <fstream>
#include "Rand.hpp"
#include "SHCWeightCollection.hpp"
#include "GEPNN.hpp"
#include "CfgFile.hpp"
#include "ProbabilisticModelElement.hpp"

class PrototypeChromosome
{
private:
	int num_functions, tail_length, weight_domain_length, gene_length, head_tail_length;
	ProbabilisticModelElement **elements;
	static CfgFile *config;
public:
	static void set_config( CfgFile& cfg );

    PrototypeChromosome();
	~PrototypeChromosome();
	void print( std::ostream &fout );
	void init();
	bool in_use( int offset, int position );

	double get_probability( GepNN *gep );
	void normalize( GepNN *gep );
	void pipe_adapt( GepNN *gep, int fitness_best, int fitness_elite );
	void pbil_adapt( GepNN *gep );
	void pipe_increment( GepNN *gep );
	void pipe_mutate( GepNN *gep, RandomNumberGenerator& rng );
	void pbil_increment( GepNN *gep );
	void pbil_mutate( GepNN *gep, RandomNumberGenerator& rng );
	void guided_mutation( GepNN *gep, GepNN *new_gep, RandomNumberGenerator& rng );
	GepNN* get_genotype( RandomNumberGenerator& rng );	
};

#endif

