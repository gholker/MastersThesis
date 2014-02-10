#ifndef DPBCPP
#define DPBCPP

#include <iostream>
#include <math.h>
#include "GEPNet.hpp"
#include "DoublePole.hpp"

enum
{
    CART_POS = 0,
    CART_VEL,
    POLE1_POS,
    POLE1_VEL,
    POLE2_POS,
    POLE2_VEL,
    BIAS
};

const int NUM_INPUTS = 7;
double state[NUM_INPUTS];

double cartpos_sum  = 0.0;
double cartv_sum    = 0.0;
double polepos_sum  = 0.0;
double polev_sum    = 0.0;
double balanced_sum = 0.0;

double jigglestep[1000];

const static double GRAVITY         = 9.8;

const static double FORCE_MAG       = 10.0;
const static double TAU             = 0.01;		  //seconds between state updates
const static double MUP             = 0.000002;
const static double MUC             = 0.0005;
const static double LENGTH_1        = 0.5;
const static double MASSPOLE_1      = 0.1;
const static double LENGTH_2        = 0.05;
const static double MASSPOLE_2      = 0.01;
const static double MASSCART        = 1.;

const static double one_degree          = 0.0174532;	/* 2pi/360 */
const static double six_degrees         = 0.1047192;
const static double twelve_degrees      = 0.2094384;
const static double fifteen_degrees     = 0.2617993;
const static double thirty_six_degrees  = 0.628329;
const static double fifty_degrees       = 0.87266;

int maxFitness = 100000;

void init( bool randomize );
int evalNet( GEPNet *net );//, int thresh );
void performAction( double output, int stepnum );
void step( double action, double *st, double *derivs );
void rk4( double f, double y[], double dydx[], double yout[] );
bool outsideBounds();
void print_data();

int double_pole_balancing_fitness( GEPNet *net, std::ostream &fout, bool printout )
{
    return evalNet( net );
}

void print_data()
{
    using namespace std;
    cout << "Cart Pos = " << state[CART_POS] << endl;
    cout << "Cart Vel = " << state[CART_VEL] << endl;
    cout << "Pole1 Pos = " << state[POLE1_POS] << endl;
    cout << "Pole1 Vel = " << state[POLE1_VEL] << endl;
    cout << "Pole2 Pos = " << state[POLE2_POS] << endl;
    cout << "Pole2 Vel = " << state[POLE2_VEL] << endl;
    cout << endl;
}

void init( bool randomize )
{
//	static int first_time = 1;

	//if( !MARKOV )
	//{
    //Clear all fitness records
    cartpos_sum=0.0;
    cartv_sum=0.0;
    polepos_sum=0.0;
    polev_sum=0.0;
	//}

	balanced_sum = 0; //Always count # balanced

	//last_hundred=false;

    state[0] = state[1] = state[3] = state[4] = state[5] = 0;
    state[2] = 0.07; // one_degree;

    /*
	if( !generalization_test )
	{
		state[0] = state[1] = state[3] = state[4] = state[5] = 0;
		state[2] = 0.07; // one_degree;
	}
	else
	{
		state[4] = state[5] = 0;
	}
	*/

    /*
	if( first_time )
 	{
		cout<<"Initial Long pole angle = %f\n"<<state[2]<<endl;;
		cout<<"Initial Short pole length = %f\n"<<LENGTH_2<<endl;
		first_time = 0;
	}
    */
}


//Faustino Gomez wrote this physics code using the differential equations from
//Alexis Weiland's paper and added the Runge-Kutta himself.
int evalNet( GEPNet *net ) //, int thresh )
{
	int steps = 0;
	double input[NUM_INPUTS];
	double output;

	//int nmarkovmax;

	//double nmarkov_fitness;

	//double jiggletotal; //total jiggle in last 100
	//int count;  //step counter

    /*
	if (nmarkov_long) nmarkovmax=100000;
	else if (generalization_test) nmarkovmax=1000;
	else nmarkovmax=1000;
    */

	init(0);

	while( steps++ < maxFitness )
	{
		input[0] = state[0] / 4.8;
		input[1] = state[1] / 2;
		input[2] = state[2] / 0.52;
		input[3] = state[3] / 2;
		input[4] = state[4] / 0.52;
		input[5] = state[5] / 2;
		input[6] = .5;

        /*
        net->load_sensors(input);

		//Activate the net
		//If it loops, exit returning only fitness of 1 step
		if( !(net->activate()) ) return 1.0;

		output=( *(net->outputs.begin()) )->activation;
        */
        //print_data();
        output = net->evaluate( input );
        //std::cout << "Output = " << output << std::endl;
		performAction( output, steps );

		if( outsideBounds() )	// if failure
			break;		// stop it now
	}
	return steps;
}


void performAction( double output, int stepnum )
{
	int i;
	double dydx[6];
	//const double EULER_TAU = TAU/4;

	/*random start state for long pole*/
	/*state[2]= drand48();   */

	/*--- Apply action to the simulated cart-pole ---*/

	for( i = 0; i < 2; ++i )
	{
		dydx[0] = state[1];
		dydx[2] = state[3];
		dydx[4] = state[5];
		step( output, state, dydx );
		rk4(  output, state, dydx, state );
	}

	//Record this state
	cartpos_sum	+= fabs( state[0] );
	cartv_sum	+= fabs( state[1] );
	polepos_sum	+= fabs( state[2] );
	polev_sum 	+= fabs( state[3] );
	if( stepnum <= 1000 )
		jigglestep[stepnum-1] = fabs( state[0] ) + fabs( state[1] ) + fabs( state[2] ) + fabs( state[3] );

	if( !(outsideBounds()) )
		++balanced_sum;

}


void step( double action, double *st, double *derivs )
{
	double force, costheta_1, costheta_2, sintheta_1, sintheta_2,
		gsintheta_1, gsintheta_2, temp_1, temp_2, ml_1, ml_2, fi_1, fi_2, mi_1, mi_2;

	force =  ( action - 0.5 ) * FORCE_MAG * 2;
	costheta_1 = cos( st[2] );
	sintheta_1 = sin( st[2] );
	gsintheta_1 = GRAVITY * sintheta_1;
	costheta_2 = cos( st[4] );
	sintheta_2 = sin( st[4] );
	gsintheta_2 = GRAVITY * sintheta_2;

	ml_1 = LENGTH_1 * MASSPOLE_1;
	ml_2 = LENGTH_2 * MASSPOLE_2;
	temp_1 = MUP * st[3] / ml_1;
	temp_2 = MUP * st[5] / ml_2;
	fi_1 = (ml_1 * st[3] * st[3] * sintheta_1) +
		(0.75 * MASSPOLE_1 * costheta_1 * (temp_1 + gsintheta_1));
	fi_2 = (ml_2 * st[5] * st[5] * sintheta_2) +
		(0.75 * MASSPOLE_2 * costheta_2 * (temp_2 + gsintheta_2));
	mi_1 = MASSPOLE_1 * (1 - (0.75 * costheta_1 * costheta_1));
	mi_2 = MASSPOLE_2 * (1 - (0.75 * costheta_2 * costheta_2));

	derivs[1] = (force + fi_1 + fi_2)
		/ (mi_1 + mi_2 + MASSCART);

	derivs[3] = -0.75 * (derivs[1] * costheta_1 + gsintheta_1 + temp_1)
		/ LENGTH_1;
	derivs[5] = -0.75 * (derivs[1] * costheta_2 + gsintheta_2 + temp_2)
		/ LENGTH_2;

}

void rk4( double f, double y[], double dydx[], double yout[] )
{
	int i;
	double hh, h6, dym[6], dyt[6], yt[6];


	hh = TAU * 0.5;
	h6 = TAU / 6.0;

	for( i = 0; i <= 5; i++ )
		yt[i] = y[i] + hh * dydx[i];
	step( f, yt, dyt );

	dyt[0] = yt[1];
	dyt[2] = yt[3];
	dyt[4] = yt[5];

	for( i = 0; i <= 5; i++ )
		yt[i] = y[i] + hh * dyt[i];
	step( f, yt, dym );

	dym[0] = yt[1];
	dym[2] = yt[3];
	dym[4] = yt[5];

	for( i = 0; i <= 5; i++ )
	{
		yt[i] = y[i] + TAU * dym[i];
		dym[i] += dyt[i];
	}
	step( f, yt, dyt );

	dyt[0] = yt[1];
	dyt[2] = yt[3];
	dyt[4] = yt[5];
	for( i = 0; i <= 5; i++ )
		yout[i] = y[i] + h6 * ( dydx[i] + dyt[i] + 2.0 * dym[i] );
}

bool outsideBounds()
{
	const double failureAngle = thirty_six_degrees;

	return
		state[0] < -2.4              ||
		state[0] > 2.4               ||
		state[2] < -failureAngle     ||
		state[2] > failureAngle      ||
		state[4] < -failureAngle     ||
		state[4] > failureAngle;
}

#endif
