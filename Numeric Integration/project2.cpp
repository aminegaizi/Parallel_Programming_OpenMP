#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <stdio.h>

// setting the number of threads:
#ifndef NUMT
#define NUMT            1
#endif

// setting the number of nodes
#ifndef NUMNODES
#define NUMNODES     	4
#endif

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES        256
#endif

//Setting the edges values of the superquadric 
#define XMIN		-1.
#define XMAX		1.
#define YMIN		-1.
#define YMAX		1.


float Height(int, int);



int main( int argc, char*argv[])
{
	double maxPerformance = 0.;	//Declaration of the variable holding the value of the max performance

	double volume = 0.;
	#ifndef _OPENMP						
		fprintf( stderr, "No OpenMP support!\n" );
		return 1;
	#endif

	omp_set_num_threads( NUMT );    // set the number of threads to use in the for loop:

	float fullTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
				( ( YMAX - YMIN )/(float)(NUMNODES-1) )  );


	// sum up the weighted zs into the variable "volume"
	// using an OpenMP for loop and a reduction:

       	// looking for the maximum performance:
       	for( int t = 0; t < NUMTRIES; t++ )
	{
		
	 	volume = 0.;		//Declaration of the variable holding the value of the volume
		double time0 = omp_get_wtime( ); //Get the time at the start of the processing 

		#pragma omp parallel for default(none) shared(fullTileArea, maxPerformance) reduction(+:volume)
								
		for( int i = 0; i < NUMNODES*NUMNODES; i++ )
		{
			int iu = i % NUMNODES;
			int iv = i / NUMNODES;
			float z = Height( iu, iv )*2;
			
			if( (iu == 0 && iv ==0) || (iu == 0 && iv == NUMNODES-1) || (iu == NUMNODES-1 && iv == 0) || (iu == NUMNODES - 1 && iv == NUMNODES - 1)) //If we are processing a corner 	
				volume += (fullTileArea/4)*z;
			
			else if (((iu == 0 || iu == NUMNODES -1) && (iv != 0 && iv != NUMNODES -1)) || ((iv == 0 || iv == NUMNODES - 1) && ( iu != 0 && iu != NUMNODES-1))) //If we are processing an edge
				volume += (fullTileArea/2)*z;
			
			else if ((iu != 0 && iu != NUMNODES - 1) && ((iv != 0 && iv != NUMNODES-1))) //If we are processing a full tile 
				volume += fullTileArea*z;
			
		
		//	printf(" iu = %d \t iv = %d \t  height = %f \t  volume = %lf \n", iu, iv, z, volume); 
		}
		double time1 = omp_get_wtime();  //Get the time at the end of the processing 
		double megaHeightComputedPerSecond = ((double)NUMNODES*NUMNODES) / (time1 - time0)/1000000.; //Calculate the performance in Mega Height Computed Per Second 
		if(megaHeightComputedPerSecond > maxPerformance ) //Look for the max performance during the multiple tries 
			maxPerformance = megaHeightComputedPerSecond;
	}


	printf("Number of threads: %d \t", NUMT); 		//Display the number of threads used
	printf("Number of tries: %d \t", NUMTRIES);		//Display the number of times the program was run 
	printf("Number of Nodes: %d \t", NUMNODES);		//Display the number of nodes used
	printf("The volume is: %lf \t", volume);		//Display the calculated value of the volume 
	printf("The max preformance is: %lf Mega Heights Computed Per Second \n", maxPerformance);		//Display the max performance achieved 

}	

float
Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	float N = 4.;
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float r = 1. - xn - yn;
	if( r < 0. )
	        return 0.;
	float height = pow( 1. - xn - yn, 1./(float)N );
	return height;
}
