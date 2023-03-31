#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

// Number of threads
#define NUM_THREADS 32

// Number of iterations
#define TIMES 1000

// Input Size
#define NSIZE 1
#define NMAX 1000000
//#define NMAX 16
//int Ns[NSIZE] = {16};
int Ns[NSIZE] = {1000000};


// Seed Input
 int A[NMAX];
//int A[] = {0,14,13,5,16,11,10,9,12,0,8,7,15,4,3,2,1};

// Subset
int B[NMAX];

//Auxiliar Arrays
int S[NMAX+1] = {0};
int D[NMAX+1] = {0};
int newS[NMAX+1] = {0};
int newD[NMAX+1] = {0};

void printResult(char* string, int n);

void init(int n){
	/* Initialize the input for this iteration*/
	// B <- A
	int i = 0;
	for (i = 0; i <= n; i++) {
		B[i] = A[i];
	}
}

void seq_function(int length){
	/* The code for sequential algorithm */
	// Perform operations on B

	int i,j;
	
	for (i=0; i<=length; i++) {
		S[i] = B[i];

		if (0 != S[i])
			D[i] = 1;
		else
			D[i] = 0;
	}

	for (j=0; j<=log2(length); j++) {	
		for (i=1; i<=length; i++) {		
			if (S[i] != S[S[i]]) {
				newD[i] = D[i] + D[S[i]];
				newS[i] = S[S[i]];
			} else {
				newD[i] = D[i];
				newS[i] = S[i];
			}	
		}

		for (i=1; i<=length; i++) {
			S[i] = newS[i];
			D[i] = newD[i];
		}
	}
}

void omp_function(int length, int nthreads){
	/* The code for threaded computation */
	// TODO
	int i,j, chunk;
	
	chunk = ceil(length/nthreads);

	for (i=0; i<=length; i++) {
		S[i] = B[i];

		if (0 != S[i])
			D[i] = 1;
		else
			D[i] = 0;
	}
	for (j=0; j<=log2(length); j++) {	

		#pragma omp parallel num_threads(nthreads) shared(S,D,newS,newD) private(i)
		{
			#pragma omp for schedule(dynamic,chunk)
			for (i=1; i<=length; i++) {
				if (S[i] != S[S[i]]) {
					newD[i] = D[i] + D[S[i]];
					newS[i] = S[S[i]];
				} else {
					newD[i] = D[i];
					newS[i] = S[i];
				}
			}
			#pragma omp for schedule(dynamic,chunk)
			for (i=1; i<=length; i++) {
				S[i] = newS[i];
				D[i] = newD[i];
			}	
		}
	}
	
}

int main (int argc, char *argv[])
{
	struct timeval startt, endt, result;
	int nt, t, n, c, k;
	
 	result.tv_sec = 0;
 	result.tv_usec= 0;

	/* Generate a seed input */
	srand ( time(NULL) );
	for(k=0; k<NMAX; k++){
		A[k] = rand() % NMAX;
	}



	printf("|NSize   |Iterations  |    Seq   |    Th02   |    Th04   |    Th08   |    Th16   |   Th32   |\n");

	// for each input size
	for(c=0; c<NSIZE; c++){
		n=Ns[c];
		printf("| %03d | %08d |",n,TIMES);

		/* Run sequential algorithm */
		result.tv_usec=0;
		gettimeofday (&startt, NULL);
		for (t=0; t<TIMES; t++) {
			init(n);
			seq_function(n);
		}
		gettimeofday (&endt, NULL);
		result.tv_usec = (endt.tv_sec*1000000+endt.tv_usec) - (startt.tv_sec*1000000+startt.tv_usec);
		printf(" %ld.%06ld | ", result.tv_usec/1000000, result.tv_usec%1000000);
//		printResult("sequential",n);

		/* Run threaded algorithm(s) */
		for(nt=1; nt<NUM_THREADS; nt=nt<<1){
			

			result.tv_sec=0; result.tv_usec=0;
			gettimeofday (&startt, NULL);
			for (t=0; t<TIMES; t++)
			{			
				init(n);
				omp_function(n,nt);

			}
			gettimeofday (&endt, NULL);
//			printResult("threaded",n);

 			result.tv_usec += (endt.tv_sec*1000000+endt.tv_usec) - (startt.tv_sec*1000000+startt.tv_usec);
			printf(" %ld.%06ld | ", result.tv_usec/1000000, result.tv_usec%1000000);
		}
		printf("\n");
	}
	return 0;
}

void printResult(char* string, int n){
	int i;
	printf("\n");
	for (i = 1; i <= n; i++){
		printf("S[%d]=%d,%s",i,S[i],(S[i]>9)?" ":"  ");
	}
	printf("\n");
	for (i = 1; i <= n; i++){
		printf("D[%d]=%d,%s",i,D[i],(D[i]>9)?" ":"  ");
	}
	printf("\n\n");
}
