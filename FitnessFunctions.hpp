
#ifndef FITNESS_H
#define FITNESS_H

#include "Individual.hpp"
#include <fstream>

int fitnessXOR( Individual *net, std::ostream &fout, bool printoutput );

int fitness6BitMux( Individual *net, std::ostream &fout, bool printoutput );

void retina_test_switch();
int fitnessRetina( Individual *net, std::ostream &fout, bool printoutput );

#endif

