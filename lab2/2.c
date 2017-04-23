/******************************************************************************
* FILENAME: omp_bug_for2.c
* DESCRIPTION: OpenMP program that contains bug caused by for directive (depends on thread scheduling)
* AUTHOR: Cristian Chilipirea
* DATE:	10-October-2016
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#define N 20

int main (int argc, char** argv) {
	int i, thread_id;
		int sum = 0;
	printf ("Real Sum = %i\n", (N + 1) * (N / 2));

	#pragma omp parallel private(thread_id)
	{
		thread_id = omp_get_thread_num();
		sum = 0;//daca il pun dupa nu merge
		#pragma omp barrier
		
		#pragma omp for reduction(+:sum)
		for (i = 0; i <= N; i++) 
			sum = sum + i;

		printf ("Thread %i Sum = %i\n", thread_id, sum);
	}
	return 0;
}
