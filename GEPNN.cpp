#ifndef GEPNN
#define GEPNN

#include <iostream>
#include <fstream>
#include <deque>
#include "GEPNet.hpp"
#include "GEPNN.hpp"
#include "SHCWeight.hpp"
#include "DataCollector.hpp"


GepNN::GepNN( short num_genes, short head_length, short min_arity, short max_arity, short num_inputs, short num_weights )
{
	net = NULL;
	weights = NULL;
	genes = NULL;
	weights_used = NULL;

	this->num_genes		= num_genes;
	this->min_arity		= min_arity;
	this->max_arity		= max_arity;
	setHeadLength(  head_length );
	this->num_weights	= num_weights;
	number_of_network_inputs	= num_inputs;
	num_functions = (max_arity - min_arity + 1);

	initOffset();
}

GepNN::~GepNN()
{
	if( weights )
		delete [] weights;

	if( weights_used )
		delete [] weights_used;

	if( net )
		delete net;

	deleteGenes();

	if( offset )
		delete [] offset;

}

void GepNN::setHeadLength( short hl )
{
	if( max_arity > 0 ) // MAX_ARITY MUST ALSO BE SET
	{
		head_length	= hl;
		tail_length	= ((head_length * (max_arity - 1)) + 1);
		wd_length	= (head_length * max_arity);
		gene_length	= (head_length + tail_length + wd_length);
	}
	else
		DataCollector::debug << "Max Arity Not Set, GepNN::setHeadLength" << endline;
}

void GepNN::setWeights( double *new_weights, short len )
{

	if( weights == NULL )
		weights = new double[ num_weights ];

	if( weights != NULL )
		for( int i = 0; i < num_weights && i < len; i++ )
		{
			weights[i] = new_weights[i];
		}

	if( weights_used == NULL )
	{
		weights_used = new bool[num_weights];
		for( int i = 0; i < num_weights; i++ )
			weights_used[i] = false;
	}


}

double* GepNN::getWeights()
{
	return weights;
}

bool* GepNN::getWeightsUsed()
{
	for( int i = 0; i < num_genes; i++ )
		for( int j = 0; j < getOffset(i); j++ )
		{
			weights_used[ genes[i][j + head_length + tail_length] ] = true;
		}
	return weights_used;
}

void GepNN::deleteGenes()
{
	if( genes != NULL )
	{
		for( int i = 0; i < num_genes; i++ )
			if( genes[i] != NULL )
				delete [] genes[i];
		delete [] genes;
	}
}

void GepNN::setGene( short i, short *gene )
{
	if( genes == NULL )
	{
		genes = new short*[num_genes];
		for( int i = 0; i < num_genes; i++ )
			genes[i] = NULL;
	}
	if( i < num_genes )
	{
		if( genes[i] == NULL )
			genes[i] = new short[gene_length];
		for( int j = 0; j < gene_length; j++ )
			genes[i][j] = gene[j];
	}
}

short* GepNN::getGene( short i )
{
	if( i < num_genes && genes[i] != NULL )
		return genes[i];
	else
		return NULL;
}

char* GepNN::getGeneString()
{
	int i, j;
	if( genes != NULL )
	{
		char *s = new char[ num_genes * gene_length + num_genes + 1 ];

		for( i = 0; i < num_genes; i++ )
		{
			for( j = 0; j < gene_length; j++ )
			{
				s[i * (gene_length + 1) + j] = getCharGene( genes[i][j], j >= head_length + tail_length );
			}
			s[ i * (gene_length + 1) + j ] = '\n';
		}
		s[ num_genes * gene_length + num_genes - 1] = '\0';
		return s;
	}
	else
	{
		char *s = new char[1];
		s[0] = '\0';
		return s;
	}
}

void GepNN::print( std::ostream &fout)
{
	using namespace std;

	char *genestring = getGeneString();
	fout << genestring << endl;
	delete [] genestring;

	fout.precision( 2 );
	for( int i = 0; i < num_weights; i++ )
		fout << weights[i] << ", ";
	fout << endl << flush;
}

char GepNN::getCharGene( short n, bool wd )
{
	if( !wd )
	{
		if( n < number_of_network_inputs )
			return '0' + n;
		else
		{
			int s = min_arity + (n - number_of_network_inputs) - 1;
			return 'A' + s;
		}
	}
	else
	{
		if( n < 10 )
			return n + '0';
		else if( n < 36 )
			return n - 10 + 'a';
		else if( n < 62 )
			return n - 36 + 'A';
		else
			return '.';
	}
    return '.';
}

int GepNN::getNeuronNumberOfInputs( char char_gene )
{
    char type = getCharGene( char_gene );
    int number_of_neuron_inputs = 0;
    if( type >= 'A' )
        number_of_neuron_inputs = type - 'A' + 1;
    //std::cout << "char_gene = " << char_gene << " type = " << type << " #of_neuron_inputs = " << number_of_neuron_inputs << std::endl;
    return number_of_neuron_inputs;
}

char GepNN::getNeuronType( short n )
{
	return 'A' + n;
}



void GepNN::initOffset()
{
	offset = new short[num_genes];
	for( int i = 0; i < num_genes; i++ )
		offset[i] = 0;
}

int GepNN::getOffset( short gene )
{
	if( gene < num_genes )
	{
	    if( offset[gene] < 0 || offset[gene] > wd_length || gene >= num_genes ) DataCollector::debug << "offset=" << offset[gene] << endline;
		return offset[gene];
	}
	else
		return 0;
}

GEPNet* GepNN::createNetwork( Neuron **inputs )
{
	using namespace std;

    // CONNECTOR NEURON -- OR
	double *orweights = new double[num_genes];
	for( int i = 0; i < num_genes; i++ )
		orweights[i] = 1.;
	Neuron *orNeuron = new Neuron( num_genes, orweights, num_genes );
	orNeuron->isOrNeuron = true;
	delete [] orweights;
	Neuron *last_net = NULL;

    // CREATE NETWORK FOR EACH GENE
	for( int g = 0; g < num_genes; g++ )
	{
		deque<Neuron*> neuron_deq;
		deque<double> weight_deq;
		short gos = 0; // OFFSET
		short used = 0;
		Neuron *root, *temp;
		short *gene = genes[g];
		short *wd = &genes[g][tail_length + head_length];

		// ENQUEUE WEIGHTS
		for( int i = 0; i < wd_length; i++ )
			weight_deq.push_back( weights[ wd[i] ] );

		// ENQUEUE FIRST NEURON
        if( gene[gos] >= number_of_network_inputs )
            root = new Neuron( getNeuronNumberOfInputs( gene[gos] ) );
        else
            root = inputs[gene[gos]];
		gos++;
		neuron_deq.push_back( root );

        // CREATE NETWORK
		while(		!neuron_deq.empty()
				&&	gos < tail_length + head_length )
		{
			temp = neuron_deq.front();
			neuron_deq.pop_front();
			used++;

			for( int i = 0; i < temp->getNumInputs(); i++ )
			{
				temp->setWeight( i, weight_deq.front() );
				weight_deq.pop_front();

				if( gene[gos] >= number_of_network_inputs )
					neuron_deq.push_back( new Neuron( getNeuronNumberOfInputs( gene[gos] ) ) );
				else
				    neuron_deq.push_back( inputs[gene[gos]] );
				gos++;

				temp->setLink( i, neuron_deq[gos - used - 1] );
			}
		}

        // CLEANUP
        int c = 0;
        while( !neuron_deq.empty() )
        {
            temp = neuron_deq.front();
            neuron_deq.pop_front();
			if( !temp->isInput() )
			{
				delete temp;
				c++;
			}
        }
        if( c > 0 ) DataCollector::debug << "GEPNN.CPP in GepNN::createNetwork -- neuron_deq deleted " << c << " neurons." << endline;

        c = 0;
        while( !weight_deq.empty() )
        {
            weight_deq.pop_front();
            c++;
        }

		// RECORD OFFSET
		offset[g] = gos - 1;
		// CONNECT TO OR
		orNeuron->setLink( g, root );
		last_net = root;
	}
	if( net != NULL )
		delete net;
    if( num_genes > 1 )
        net = new GEPNet( orNeuron, inputs, number_of_network_inputs );
    else
    {
		orNeuron->setLink( 0, NULL );
        net = new GEPNet( last_net, inputs, number_of_network_inputs );
        delete orNeuron;
    }
	return net;
}


void GepNN::setGeneElem( short gene, short elem, short nvalue )
{
	if( gene < num_genes && elem < gene_length )
		genes[gene][elem] = nvalue;
}

void GepNN::setWeight( short w, double nvalue )
{
	if( w < num_weights )
		weights[w] = nvalue;
}

int GepNN::getSize()
{
	int size = 0;
	for( int i = 0; i < num_genes; i++ )
		size += getOffset(i) + 1;
	return size;
}


#endif
