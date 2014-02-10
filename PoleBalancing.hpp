#ifndef PB_H
#define PB_H

#include "Individual.hpp"
#include "Rand.hpp"
#include <fstream>

extern double pole_pos;
extern double cart_pos;
extern double pole_length;

bool pole_balance_tick();
void pole_balance_reset();
void pole_balance_set_seed( long seed = -1L );
int pole_balancing_fitness( Individual *net, std::ostream &fout, bool printout );

#endif

