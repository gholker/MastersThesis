#ifndef CGP_GRAPH_HPP
#define CGP_GRAPH_HPP

#include "CfgFile.hpp"
#include "ActivationFunctions.hpp"
#include <iostream>

class CGPGraph
{
private:
	static CfgFile* config;

	double *outputs;
	double *inputs;
	int *gene;
	double *weights;
public:
	static void set_config( CfgFile& cfg );
	CGPGraph();
	~CGPGraph();
	
	int* get_gene();
	double* get_weight();
	void init( int *gene, double *weights );
	void set_inputs( double *inputs );
	void delete_inputs();
	void delete_outputs();
	void print( ostream& fout );

	double* evaluate( double *inputs );
	double evaluate_node( int index );
	void find_used_nodes( bool *nodes_used, bool *weights_used );
	void find_used( int index, bool *is_used, bool *weights_used );
};

#endif
