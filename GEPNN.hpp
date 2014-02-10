#ifndef GEPNN_H
#define GEPNN_H

#include "GEPNet.hpp"
#include <iostream>
#include <fstream>

class GepNN
{
private:
	short **genes;
	double *weights;
	bool *weights_used;
	void deleteGenes();
	short min_arity;
	short max_arity;
	short head_length;
	short tail_length;
	short wd_length;

	int number_of_network_inputs;

	short num_weights;
	short num_genes;	
	short num_functions;

	short *offset;


	GEPNet *net;

	void initOffset();
	int getNeuronNumberOfInputs( char char_gene );
public:

	GepNN(  short num_genes,
            short head_length,
            short min_arity,
            short max_arity,
            short num_inputs,
			short num_weights );
	GepNN( GepNN *cpy );
	~GepNN();
	void setHeadLength( short hl );
	void setWeights( double *weights, short len ); // WEIGHTS ARE COPIED
	double* getWeights();
	bool* getWeightsUsed();
	void setGene( short i, short *gene );
	void setGene( short index, char *gene );
	short* getGene( short i );
	char* getGeneString();
	static char getNeuronType( short n );
	char getCharGene( short n, bool wd = false );
	GEPNet* createNetwork( Neuron **inputs );
	int getOffset( short gene );
	void setGeneElem( short gene, short elem, short nvalue );
	void setWeight( short w, double nvalue );

	int getSize();

	int fitness;
	void print( std::ostream &fout );
	short gene_length;


};

#endif

