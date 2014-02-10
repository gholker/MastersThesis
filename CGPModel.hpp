#ifndef CGP_MODEL_HPP
#define CGP_MODEL_HPP

#include "CfgFile.hpp"
#include "ProbabilisticModelElement.hpp"
#include "Rand.hpp"
#include "CGPModelNode.hpp"
#include "CGPGraph.hpp"
#include <iostream>

class CGPModel
{
private:
    static CfgFile *config;
	
    // for the nodes
    int number_of_nodes;
    CGPModelNode *node_elements;

    // for the outputs
    ProbabilisticModelElement *output_elements;
	int *genotype;    
	double probability;
public:
    static void set_config( CfgFile& cfg );

    CGPModel();
    ~CGPModel();
	void delete_genotype();
	int length();
	void print( ostream& fout );

    int* get_genotype( RandomNumberGenerator& rng );
	void guided_mutation( int *elite_gene, int *new_gene, RandomNumberGenerator& rng );
	double get_probability( int *gene );
	void normalize( int *gene );
    void pipe_mutate( RandomNumberGenerator& rng );
    void pipe_increment( int *gene );
	void pipe_adapt( int *gene, int fitnessBest, int fitnessElite );
	void pbil_mutate( RandomNumberGenerator& rng );
    void pbil_increment( int *gene );
	void pbil_adapt( int *gene );

	void* traverse_model( void *param, int directive );
};

#endif

