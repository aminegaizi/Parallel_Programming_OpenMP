#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>


unsigned int seed = 0;

int	NowYear;		// 2020 - 2025
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
float 	NowInfectiousLevel;	// Infectious level of the environment

const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;



/* ----------- Function prototypes ------------ */
float SQR( float x ); 		
float Ranf( unsigned int *seedp,  float low, float high );
int Ranf( unsigned int *seedp, int ilow, int ihigh );

void Grain();
void GrainDeer();
void Watcher();
void MyAgent();

float InchesToCm(float Height); 		// Converts distances from Inches to Cm
float FareiheitToCelsius(float Temperature);	// Converts temperatures from Farenheit to Celsius


int main( int argc, char*argv[])
{
	// starting date and time:
	NowMonth =    0;
	NowYear  = 2020;

	// starting state 
	NowNumDeer = 1.;
	NowHeight = 1.;
	NowInfectiousLevel = 0.;	// Infectious level initally set to 0

	omp_set_num_threads( 4 );	// same as # of sections
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
			MyAgent( );	// your own
		} 
	}       // implied barrier -- all functions must return in order
		// to allow any of them to get past here


}

void MyAgent()
{
	while (NowYear < 2026)
	{
		float nextInfectiousLevel = NowInfectiousLevel; 	// Get the current value of the infectious level
		if(NowTemp < 50 && NowPrecip > 11)			//If the temperature is below 50°F and precipitation over 11cm 
			nextInfectiousLevel = nextInfectiousLevel + 0.25; // The environment is 25% more infectious 
		else if(NowInfectiousLevel > 0)				 // Else, decrease the infectiousness by 2.5%	
			nextInfectiousLevel = nextInfectiousLevel - 0.025;	
			if(nextInfectiousLevel < 0)			// Make sure to not have a negative value
				nextInfectiousLevel = 0.;
		#pragma omp barrier 
		
		NowInfectiousLevel = nextInfectiousLevel;		//Update state of the system
		
		#pragma omp barrier

		#pragma omp barrier

	}
}
void Watcher()
{

	while (NowYear < 2026)
	{
		// Compute a temporary next value based on the current state of the simulation
		#pragma omp barrier 		//Done Computing Barrier

		#pragma omp barrier		//Done Assigning Barrier
	
		float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
		float temp = AVG_TEMP - AMP_TEMP * cos( ang );
		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	
		NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );
        	NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );


		//Display the state of the system
		printf("Current Year: %d,	Current Month: %d	Infectious Level: %f \n", NowYear, NowMonth, NowInfectiousLevel);
		printf("Temperature: %f,	Precipitation: %f \n", FareiheitToCelsius(NowTemp), NowPrecip);
		printf("Nb of Deers: %f, 	Height of Grain %f \n", (float) NowNumDeer, InchesToCm(NowHeight));
		
		
		printf("\n");
		printf(" ------- Next Month ---------");
		printf("\n");
        	if( NowPrecip < 0. )
                	NowPrecip = 0.;	
		
		if(NowMonth >= 11)	//Increase the number of months and years
		{	
			NowMonth = 0;
			NowYear += 1;
		}
		else	
			NowMonth += 1;			
		
		#pragma omp barrier 		//Done Printing barrier
	}
}	

void GrainDeer()
{

	while (NowYear < 2026)
	{
		int nextNumDeer = NowNumDeer;
		// Compute a temporary next value based on the current state of the simulation
		if(NowNumDeer > NowHeight) 		// If there is not enough food for the population to expand
			nextNumDeer = (nextNumDeer - 1) - round(NowNumDeer*NowInfectiousLevel) ;	//Decrease Nb of deers and measure the impact of the infectious disease
		else if (NowNumDeer < NowHeight)	// If there is enough food for the population to expand 
			nextNumDeer = (nextNumDeer + 1) - round(NowNumDeer*NowInfectiousLevel) ;	//Increase Nb of deers and measure the impact of the infectious disease
		#pragma omp barrier 		//Done Computing Barrier
		NowNumDeer = nextNumDeer;			//Update state variable

		#pragma omp barrier		//Done Assigning Barrier
		
		#pragma omp barrier 		//Done Printing barrier
	}
}

void Grain()
{

	while (NowYear < 2026)
	{
		//Measure the impact of the temperature and precipitation on the height of grain available
		float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
		float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

		float	nextHeight = NowHeight;
		nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
		nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;	
	
		if( nextHeight < 0)
			nextHeight = 0;	
		#pragma omp barrier 		//Done Computing Barrier
		NowHeight = nextHeight;		//Update state of the system


		#pragma omp barrier		//Done Assigning Barrier
			
		#pragma omp barrier 		//Done Printing barrier
	}
}


float
SQR( float x )
{
        return x*x;
}

float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

int
Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}

float FareiheitToCelsius(float Temperature)
{
	return (5./9.)*(Temperature-32);
}


float InchesToCm(float Height)
{
	return Height * 2.54;
}


