
#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include "GEPNN.hpp"
#include "CfgFile.hpp"
#include "BitwiseWeights.hpp"
#include "CGPGraph.hpp"

class Individual
{
private:
    static CfgFile *config;

    // GEP
    GepNN **gep_genes;
    bool gep_genes_initialized;
    MIMO_FF_GEPNet *gepnet;
    bool gepnets_initialized;

    // CGP
    CGPGraph cgp_graph;
	bool *cgp_weights_used;

    // Bitwise Weights
    bitwise_weight_value **weight_values;
	double **weights;
public:
    int fitness;

    Individual();
    ~Individual();
    static void set_config( CfgFile& cfg );

    const double* evaluate( double *inputs );

    // GEP
    void delete_weight_values();
    void delete_gep_genes();
    void init_gep_genes();
    void set_gep_gene( GepNN *gene, int i );
    GepNN** get_gep_genes();
    void delete_gepnet();
    void set_gepnet( MIMO_FF_GEPNet *net );
    MIMO_FF_GEPNet* get_gepnet();

    // CGP
	void cgp_graph_init( int *gene, double *weights );
	int* get_cgp_array();

    // BITWISE WEIGHTS
    void set_weight_values( int index, bitwise_weight_value *values );
    bitwise_weight_value* get_weight_values( int index );

	// SHC Weights
	void set_weights( int index, double *weights );
	double* get_weights( int index );
	void delete_weights();

	// weights used
	bool* get_weights_used( int index );
	void delete_weights_used();

    void print( ostream& fout );
};

#endif
