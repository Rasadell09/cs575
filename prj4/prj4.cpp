#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <iostream>

using namespace std;

const float GRAIN_GROWS_PER_MONTH =			15.0;
const float ONE_DEER_EATS_PER_MONTH =		0.5;
const int ONE_TIGHER_EATS_PER_MONTH =		1;

const float AVG_PRECIP_PER_MONTH =			6.0;
const float AMP_PRECIP_PER_MONTH =			6.0;
const float RANDOM_PRECIP =					2.0;

const float AVG_TEMP =						55.0;
const float AMP_TEMP =						20.0;
const float RANDOM_TEMP =					10.0;

const float MIDTEMP = 						40.0;
const float MIDPRECIP = 					10.0;

int		NowYear;		// 2016 - 2021
int		NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int		NowNumDeer;		// number of deer in the current population
int		NowTigers;		// number of tigers in the current population (MyAgent)

int		NowIndex;

float
Ranf( float low, float high )
{
	float r = (float) rand( );               // 0 - RAND_MAX

	return (   low  +  r * ( high - low ) / (float)RAND_MAX   );
}


int
Ranf( int ilow, int ihigh )
{
	float low = (float)ilow;
	float high = (float)ihigh + 0.9999f;

	return (int)(  Ranf(low, high) );
}

void GrainDeer()
{
	while ( NowYear <= 2021 )
	{
		int tmpNumDeer = NowNumDeer;

		tmpNumDeer -= NowTigers * ONE_TIGHER_EATS_PER_MONTH;

		if ((tmpNumDeer * ONE_DEER_EATS_PER_MONTH) > NowHeight)
			tmpNumDeer -= 1;

		if ((tmpNumDeer * ONE_DEER_EATS_PER_MONTH) < NowHeight)
			tmpNumDeer += 4;

		if (tmpNumDeer < 0)
			tmpNumDeer = 0;
		#pragma omp barrier

		NowNumDeer = tmpNumDeer;
		#pragma omp barrier

		#pragma omp barrier
	}
}

void Grain()
{
	while ( NowYear <= 2021 )
	{
		int tmpGrainHeight = NowHeight;

		float tf = exp(-pow((NowTemp - MIDTEMP) / 10.0, 2)); // use the equation
		float pf = exp(-pow((NowPrecip - MIDPRECIP) / 10.0, 2)); // use the equation

		tmpGrainHeight += tf * pf * GRAIN_GROWS_PER_MONTH;
		tmpGrainHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

		if (tmpGrainHeight < 0.0)
			tmpGrainHeight = 0.0;
		#pragma omp barrier

		NowHeight = tmpGrainHeight;
		#pragma omp barrier

		#pragma omp barrier
	}
}

void Watcher()
{
	while ( NowYear <= 2021 )
	{
		#pragma omp barrier

		#pragma omp barrier

		float printTemp = (5. / 9.) * (NowTemp - 32);

		printf("#%02d, %02d/%d: ", NowIndex, NowMonth, NowYear);
		printf("Temperature: %6.2f   Precipitation: %6.2f", printTemp, NowPrecip);
		printf("\tGrainDeer: %02d   Tigher: %02d   Grain: %6.2f\n", NowNumDeer, NowTigers, NowHeight);

		NowIndex += 1;
		NowMonth += 1;
		if (NowMonth > 11) {
			NowYear += 1;
			NowMonth = 0;
		}

		float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

		float temp = AVG_TEMP - AMP_TEMP * cos( ang );
		NowTemp = temp + Ranf( -RANDOM_TEMP, RANDOM_TEMP );

		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
		NowPrecip = precip + Ranf( -RANDOM_PRECIP, RANDOM_PRECIP );

		if ( NowPrecip < 0. )
			NowPrecip = 0.;
		#pragma omp barrier
	}
}

void MyAgent()
{
	while ( NowYear <= 2021 )
	{
		int tmpNumTiger = NowTigers;

		if (NowNumDeer >= 7) {
			if (tmpNumTiger == 0) {
				tmpNumTiger = 1;
			}
			else {
				if ((tmpNumTiger * ONE_TIGHER_EATS_PER_MONTH) >= (NowNumDeer / 2))
					tmpNumTiger -= 1;

				if ((tmpNumTiger * ONE_TIGHER_EATS_PER_MONTH) < (NowNumDeer / 2))
					tmpNumTiger += 1;
			}
		}
		else {
			tmpNumTiger -= 2;
		}

		if (tmpNumTiger < 0) {
			tmpNumTiger = 0;
		}
		#pragma omp barrier

		NowTigers = tmpNumTiger;
		#pragma omp barrier

		#pragma omp barrier
	}
}

int main(int argc, char ** argv)
{
	srand(time(NULL));

	// starting date and time:
	NowMonth = 0;
	NowYear  = 2016;
	NowIndex = 0;

	// starting state (feel free to change this if you want):
	NowNumDeer = 7;
	NowHeight =  20.;
	NowTigers = 1;

	float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

	float temp = AVG_TEMP - AMP_TEMP * cos( ang );
	NowTemp = temp + Ranf( -RANDOM_TEMP, RANDOM_TEMP );

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf( -RANDOM_PRECIP, RANDOM_PRECIP );
	if ( NowPrecip < 0. )
		NowPrecip = 0.;

	omp_set_num_threads( 4 );
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			GrainDeer( );
		}

		#pragma omp section
		{
			Grain( );
		}

		#pragma omp section
		{
			Watcher( );
		}

		#pragma omp section
		{
			MyAgent( );
		}
	}       // implied barrier -- all functions must return in order to allow any of them to get past here

	return 0;
}