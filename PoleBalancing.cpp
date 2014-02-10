#ifndef PB_CPP
#define PB_CPP

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include "PoleBalancing.hpp"
#include "GEPNet.hpp"


long pb_seed;

double pole_pos = 0.1;
double pole_vel = 0.;
double pole_acc = 0.;
double cart_pos = 0.;
double cart_vel = 0.;
double cart_acc = 0.;
double g = -9.81;
double neat_gravity = 9.8;
double cart_mass = 1.;
double pole_mass = .1;
double pole_length = .5;
double sim_time = 0.;	// seconds
double force = 1.;
double track_limit = 2.4;
double failure_angle = .209;
double time_step = 0.02 ; // 50 Hz

double k1,k2,k3,k4;

void print_data( std::ostream &fout )
{
	using namespace std;

	fout << "Angle=" << pole_pos << endl;
	fout << "vel  =" << pole_vel << endl;
	fout << "acc  =" << pole_acc << endl;
	fout << "----------" << endl;
	fout << "CartP=" << cart_pos << endl;
	fout << "Vel  =" << cart_vel << endl;
	fout << "Acc  =" << cart_acc << endl;
	fout << "----------" << endl;
	fout.precision( 3 );
	fout << "sim_time=" << sim_time << endl;
	fout << "force=" << force << endl;
	fout << endl;
}

void calc_pole_acc()
{
	double numerator;
	double denominator;
	numerator = g * sin( pole_pos ) + cos( pole_pos );
	numerator *= (	- force
					- pole_mass * pole_length * pole_vel * pole_vel
					* sin( pole_pos ));
	numerator /= ( cart_mass + pole_mass );

	denominator = pole_mass * cos( pole_pos ) * cos( pole_pos );
	denominator /= ( cart_mass + pole_mass );
	denominator = 4. / 3. - denominator;
	denominator *= pole_length;

	pole_acc = numerator / denominator;
}

void calc_cart_acc()
{
	double result;
	result = pole_vel * pole_vel * sin( pole_pos );
	result -= pole_acc * cos( pole_pos );
	result *= pole_mass * pole_length;
	result = force - result;
	result /= ( cart_mass + pole_mass );

	cart_acc = result;
}

double sgn( double x )
{
	if( x < 0 )
		return -1.;
	if( x == 0. )
		return 0.;
	if( x > 0 )
		return 1.;
    return 0.;
}


void pole_balance_reset()
{
	pole_pos = 0.0;
	pole_vel = 0.;
	pole_acc = 0.;
	cart_pos = 0.;
	cart_vel = 0.;
	cart_acc = 0.;
	g = -9.81;
	cart_mass = 1.;
	pole_mass = .1;
	pole_length = .5;
	sim_time = 0.;	// seconds
	force = 10.;
	track_limit = 2.4;
	failure_angle = .209; //const static double thirty_six_degrees= 0.628329;
	time_step = 0.02 ; // 50 Hz
}

bool pole_balance_tick()
{

	calc_pole_acc();
	calc_cart_acc();
	cart_pos = cart_pos + time_step * cart_vel;
	cart_vel = cart_vel + time_step * cart_acc;

	pole_pos = pole_pos + time_step * pole_vel;
	pole_vel = pole_vel + time_step * pole_acc;

	sim_time += time_step;

    return !( ( fabs( pole_pos ) > failure_angle ) || ( fabs( cart_pos ) > track_limit ) );
}


bool pole_balance_tick_NEAT()
{

    double costheta = cos( pole_pos );
    double sintheta = sin( pole_pos );
    double total_mass = ( pole_mass + cart_mass );
    double temp = ( force + pole_mass * pole_length * pole_vel * pole_vel * sintheta) / total_mass;


    pole_acc = (neat_gravity * sintheta - costheta * temp) / (pole_length * (1.3333333333 - pole_mass * costheta * costheta / total_mass ));

    cart_acc  = temp - pole_mass * pole_length * pole_acc * costheta / total_mass ;


    cart_pos  += time_step * cart_vel;
    cart_vel  += time_step * cart_acc;
    pole_pos  += time_step * pole_vel;
    pole_vel  += time_step * pole_acc;

    sim_time += time_step;

    return !( ( fabs( pole_pos ) > failure_angle ) || ( fabs( cart_pos ) > track_limit ) );
}


void pole_balance_set_seed( long seed )
{
	pb_seed = seed;
}




//	   I adapted this code from Ken Stanley's NEAT code
//     cart_and_pole() was take directly from the pole simulator written
//     by Richard Sutton and Charles Anderson.
int pole_balancing_fitness( Individual *net, ostream& fout, bool printout )
{
	//double one_degree = 0.0174532;
	//double six_degrees =0.1047192;
	double twelve_degrees = 0.2094384;

	int steps = 0, max_steps = 1500;
	double inputs[5];

	RandomNumberGenerator rng( pb_seed );


	pole_balance_reset();

	cart_pos = rng.getEquilikely( 0, 4800 ) / 1000. - 2.4;
	cart_vel = rng.getEquilikely( 0, 2000 ) / 1000. - 1.;
	pole_pos = rng.getEquilikely( 0, 400 )  / 1000. - .2;
	pole_vel = rng.getEquilikely( 0, 3000 ) / 1000. - 1.5;

	pb_seed = rng.getCurrentSeed();

	/*
	cart_pos = cart_vel = pole_pos = pole_vel = 0.;
	pole_pos = six_degrees;
	pole_vel = .5;
	*/

	while( steps++ < max_steps )
	{
		inputs[0] = 1.0;  //Bias
		inputs[1] = ( cart_pos + 2.4 ) / 4.8;;
		inputs[2] = ( cart_vel + .75 ) / 1.5;
		inputs[3] = ( pole_pos + twelve_degrees ) / .41;
		inputs[4] = ( pole_vel + 1.0 ) / 2.0;

        const double *return_values = net->evaluate( inputs );
        if( return_values[0] >= return_values[1] )
            force = 10;
        else
            force = -10;

		//std::cout << steps << " cart:pos,vel; pole:pos,vel; force = " << cart_pos << ", " << cart_vel << "; " << pole_pos << ", " << pole_vel << "; " << force << std::endl;
		//std::cout << steps << " cart:pos,vel; pole:pos,vel; force = " << inputs[1] << ", " << inputs[2] << "; " << inputs[3] << ", " << inputs[4] << "; " << force << std::endl;

		if( printout )
			print_data( fout );

		if( !pole_balance_tick_NEAT() )
		{
			return steps - 1;
		}
	}
	return steps - 1;
}

#endif


