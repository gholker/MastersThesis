#ifndef CGP_MODEL_NODE_HPP
#define CGP_MODEL_NODE_HPP

#include "CfgFile.hpp"
#include "ProbabilisticModelElement.hpp"
#include "Rand.hpp"
#include <iostream>

class CGPModelNode
{
private:
	static CfgFile* config;
	RandomNumberGenerator rng;

	short number_of_elements;
	ProbabilisticModelElement *elements;
	int *gene;
public:
	static void set_config( CfgFile& cfg );
	static int length();

	CGPModelNode();
	~CGPModelNode();
	void delete_gene();
	void init( int number_of_connections );
	void print( ostream& fout );
	int get_value( int gene_value, int column );

	int* get_genotype( RandomNumberGenerator& rng );
	double get_probability( int *gene, int column );
	void normalize( int *gene, int column );
    void pipe_mutate( int size, RandomNumberGenerator& rng );
    void pipe_increment( int *gene, int column );
    void pbil_mutate( RandomNumberGenerator& rng );
    void pbil_increment( int *gene, int column );
};

#endif
