#include <omp.h>
#include <stdio.h>
#include <math.h>

#define NUMT	        1
#define SIZE       	16384	// you decide
#define NUMTRIES        200	// you decide

float A[SIZE];
float B[SIZE];
float C[SIZE];
double time_difference = 0;

int
main( )
{
#ifndef _OPENMP
        fprintf( stderr, "OpenMP is not supported here -- sorry.\n" );
        return 1;
#endif
	double time_precision = omp_get_wtick();
	printf("Time precision is %.9lf\n", time_precision);
	// inialize the arrays:
	for( int i = 0; i < SIZE; i++ )
	{
		A[ i ] = 1.;
		B[ i ] = 2.;
	}
        omp_set_num_threads( NUMT );
        fprintf( stderr, "Using %d threads\n", NUMT );

        double maxMegaMults = 0.;
        for( int t = 0; t < NUMTRIES; t++ )
        {
                double time0 = omp_get_wtime( );

                #pragma omp parallel for
                for( int i = 0; i < SIZE; i++ )
                {
                        C[i] = A[i] * B[i];
                }

                double time1 = omp_get_wtime( );
                double megaMults = (double)SIZE/(time1-time0)/1000000.;
                if( megaMults > maxMegaMults )
                        maxMegaMults = megaMults;
		time_difference = (time1 - time0)/time_precision;
        }
	printf( "Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults );
	printf(" Execution time is: %.1lf nS \n", time_difference);
	float S = 3.62;
	float Fp = (4./3.)*(1.-(1./S));
	printf("Fp = %.4f\n", Fp);
	// note: %lf stands for "long float", which is how printf prints a "double"
	//        %d stands for "decimal integer", not "double"
	return 0;
}	
	
