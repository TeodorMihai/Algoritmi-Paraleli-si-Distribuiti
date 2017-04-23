/******************************************************************************
* FILENAME: omp_bug_deadlock.c
* DESCRIPTION: OpenMP program that contains deadlock
* AUTHOR: Cristian Chilipirea
* DATE:	10-October-2016
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#define N 10000000 // Try a very small N. Does it still deadlock every time?

int main (int argc, char** argv) {
	int number_of_threads, i, a, b;

	omp_lock_t locka, lockb;
	omp_init_lock(&locka);
	omp_init_lock(&lockb);

	#pragma omp parallel private(i) num_threads(2)
	{
		#pragma omp master
		{
			number_of_threads = omp_get_num_threads();
			printf("Number of threads is %i\n", number_of_threads);
		}

		int thread_id = omp_get_thread_num();
		printf("Thread %i starting\n", thread_id);
		#pragma omp barrier

		#pragma omp sections
		{
			#pragma omp section
			{
				omp_set_lock(&locka);
				for(i = 0; i < N; i++)
					a += 1;
				
				omp_set_lock(&lockb);
				printf("Thread %i a+b is %i\n", thread_id, a + b);
				omp_unset_lock(&locka);
				omp_unset_lock(&lockb);
			}

			#pragma omp section
			{	

				omp_set_lock(&locka);
				omp_set_lock(&lockb);
				for(i = 0; i < N; i++)
					b += 2;
				
				printf("Thread %i a+b is %i\n", thread_id, a + b);
				omp_unset_lock(&locka);
				omp_unset_lock(&lockb);
			}
		}
	}
	return 0;
}