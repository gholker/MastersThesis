
#ifndef EXPERIMENT_H
#define EXPERIMENT_H


#include "Rand.hpp"
#include "GEPNet.hpp"
#include "CfgFile.hpp"
#include "DataCollector.hpp"

#include <iostream>
#include <fstream>

class DataCollector;


class Experiment
{
private:
    static CfgFile *config;	

public:
	static void set_config( CfgFile& cfg );

	Experiment();
	~Experiment();
	void run( DataCollector& dc, RandomNumberGenerator& rng );
};

#endif

