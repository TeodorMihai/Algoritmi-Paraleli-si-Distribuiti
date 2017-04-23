/******************************************************************************
* FILENAME: omp_for.c
* DESCRIPTION: OpenMP program that demonstrates for work-sharing
* AUTHOR: Cristian Chilipirea
* DATE:	10-October-2016
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#define N 20

int main (int argc, char** argv) {
	int number_of_threads, thread_id, i;
	int a[N], b[N], c[N];

	for (i = 0; i < N; i++) {
		a[i] = i;
		b[i] = i;
	}

	#pragma omp parallel shared(a, b, c) private(i, number_of_threads, thread_id)
	{
		thread_id = omp_get_thread_num();

		#pragma omp for // Each thread receives a number of iterations from the for
		for (i = 0; i < N; i++)	{
			c[i] = a[i] + b[i];
			printf("thread_id = %i \t c[%3i] = %i \n", thread_id, i, c[i]);
		}

		#pragma omp master
		{
			number_of_threads = omp_get_num_threads();
			printf("Number of threads = %d\n", number_of_threads);
		}
	}
	return 0;
}
