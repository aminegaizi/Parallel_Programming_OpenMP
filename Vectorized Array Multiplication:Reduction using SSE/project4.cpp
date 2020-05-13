#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <xmmintrin.h>


//Constant variables
#ifndef SSE_WIDTH
#define SSE_WIDTH	4
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE 	1024		//Array Size
#endif

#ifndef NUMTRIES
#define NUMTRIES	500		//Number of tries to find max performance
#endif

#ifndef NUMT
#define NUMT		1
#endif

#ifndef NUM_ELEMENTS_PER_CORE
#define NUM_ELEMENTS_PER_CORE	ARRAYSIZE / NUMT
#endif 

//Function Prototypes
float SimdMulSum( float *a, float *b, int len );
float NonSimdMulSum( float *a, float *b, int len);

float A[ARRAYSIZE];
float B[ARRAYSIZE];


int main ()
{

	#ifndef _OPENMP
		fprintf( stderr, "OpenMP is not supported here. \n");
		return 1;
	#endif
	
	for (int  i = 0; i < ARRAYSIZE; i++)
	{
		A[i] = 1.;
		B[i] = 2.;
	}
	float result = 0.;
	double time_precision = omp_get_wtick();

	double maxMegaMults = 0.;
	double maxMegaMults_2 = 0.;
	
	// -------------------- SIMD Calculation -----------------------//
	for (int t = 0; t < NUMTRIES; t++)						//Repeat a NUMTRIES times to get the best performance
	{
		double time0 = omp_get_wtime();						//Get starting time

		result = SimdMulSum(A,B,ARRAYSIZE);					// SIMD Calculation
		
		double time1 = omp_get_wtime();						//Get Ending Time
		double megaMults = (double)ARRAYSIZE/(time1-time0)/1000000.;		//Compue performance
		if(megaMults > maxMegaMults)						//Get Max performance
			maxMegaMults = megaMults;
	}	

	printf( " SIMD Peak performance = %8.2lf MegaMult/Sec \t", maxMegaMults);	
	printf( " SIMD Calculated sum is : %f \n", result); 

	// -------------------- 1 thread Calculation -----------------------//
	float result_2 = 0.;								//Variable holding Multiplication Sum Result

	for ( int t = 0; t < NUMTRIES; t++)						//Repeat a NUMTRIES times to get the best performance
	{
		double time0 = omp_get_wtime();		 				//Get starting time

		result_2 = NonSimdMulSum( A, B, ARRAYSIZE);	 			// 1 thread non SIMD Calculation

		double time1 = omp_get_wtime();						//Get Ending time
		double megaMults = (double)ARRAYSIZE/(time1-time0)/1000000.;		//Compute performance
                if(megaMults > maxMegaMults_2)						//Get Max performance
                          maxMegaMults_2 = megaMults;
	}
	float speedup = (maxMegaMults/maxMegaMults_2); 					//Calculate speedup SIMD compared to 1 thread
	printf( " Non SIMD Peak performance = %8.2lf MegaMult/Sec \t", maxMegaMults_2);
	printf( " Non SIMD Calculated sum is : %f \n", result_2); 
	printf( " Speed up: %f \n", speedup);


	// -------------------- SIMD and Multithreading Calculation-----------------------//
	omp_set_num_threads( NUMT );		//Set number of threads
	double maxMegaMults_3 = 0.;		//Variable holding max performance
	float result_3 = 0.;			//Variable holding Multiplication Sum Result

	for ( int t = 0; t < NUMTRIES; t++)						//Repeat a NUMTRIES times to get the best performance
	{
		result_3 = 0;
		double time0 = omp_get_wtime();						//Get starting time
		#pragma omp parallel reduction(+:result_3)
		{
			int first = omp_get_thread_num() * NUM_ELEMENTS_PER_CORE; 	
			result_3 += SimdMulSum(&A[first], &B[first], NUM_ELEMENTS_PER_CORE);		//Simd Calculation with multiple threads 
		}

		double time1 = omp_get_wtime();						//Get Ending time
		double megaMults = (double)ARRAYSIZE/(time1-time0)/1000000.;		//Compute performance 
                if(megaMults > maxMegaMults_3)						//Get Max performance 
                          maxMegaMults_3 = megaMults;
	}

	float speedup_2 = (maxMegaMults_3/maxMegaMults_2); 				//Calculate speedup compared to 1 thread
	printf( " Multithreading and SIMD Peak performance = %8.2lf MegaMult/Sec \t", maxMegaMults_3);
	printf( " Multithreading and SIMD Calculated sum is : %f \n", result_3); 
	printf( " Speed up Multithreading + SIMD / 1 thread: %f \n", speedup_2);

	// -------------------- Multithreading only Calculation-----------------------//
	omp_set_num_threads( NUMT );		//Set number of threads	
	double maxMegaMults_4 = 0.;		//Variable holding max performance
	float result_4 = 0.;			//Variable holding Multiplication Sum Result

	for ( int t = 0; t < NUMTRIES; t++)						//Repeat a NUMTRIES times to get the best performance
	{
		result_4 = 0;
		double time0 = omp_get_wtime();						//Get starting time
		#pragma omp parallel reduction(+:result_4)
		{
			int first = omp_get_thread_num() * NUM_ELEMENTS_PER_CORE;
			result_4 += NonSimdMulSum(&A[first], &B[first], NUM_ELEMENTS_PER_CORE);		//Multiple thread calculation
		}

		double time1 = omp_get_wtime();						//Get Ending time
		double megaMults = (double)ARRAYSIZE/(time1-time0)/1000000.;
                if(megaMults > maxMegaMults_4)
                          maxMegaMults_4 = megaMults;
	}

	float speedup_3 = (maxMegaMults_4/maxMegaMults_2); 
	printf( " Multithreading Peak performance = %8.2lf MegaMult/Sec \t", maxMegaMults_4);
	printf( " Multithreading Calculated sum is : %f \n", result_4); 
	printf( " Speed up Multithreading / 1 thread : %f \n", speedup_3);


	
}		
		
float NonSimdMulSum( float *a, float *b, int len)
{
	float sum_2[4] = {0., 0., 0., 0.};
	for (int j = 0; j < len; j++)
	{
		sum_2[0] += A[j] * B[j];
	}
	return sum_2[0] + sum_2[1] + sum_2[2] + sum_2[3];	
}

float
SimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	register float *pa = a;
	register float *pb = b;

	__m128 ss = _mm_loadu_ps( &sum[0] ); 		//Declare a 128 bit 4 float word
	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		//Add two 128 bit4 float words: ss and multiplication of arguments pa and pb 
		ss = _mm_add_ps( ss, _mm_mul_ps( _mm_loadu_ps( pa ), _mm_loadu_ps( pb ) ) );
		pa += SSE_WIDTH;
		pb += SSE_WIDTH;
	}
	_mm_storeu_ps( &sum[0], ss );	 		//Store a 128 bit 4 float word into memory

	for( int i = limit; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
}
