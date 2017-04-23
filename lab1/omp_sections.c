/******************************************************************************
* FILENAME: omp_sections.c
* DESCRIPTION: OpenMP program that demonstrates sections work-sharing
* AUTHOR: Cristian Chilipirea
* DATE:	10-October-2016
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#define N 20

int main (int argc, char** argv) {
	int i, thread_id;
	int a[N], b[N], c[N];

	for (i=0; i < N; i++) {
		a[i] = i;
		b[i] = 3;
	}

	#pragma omp parallel shared(a,b,c) private(i, thread_id)
	{
		thread_id = omp_get_thread_num();

		#pragma omp sections
		{
			/*
				Each section goes to a thread;
				What happens if there are more sections than threads?
				Or less?
			*/

			#pragma omp section
			for (i = 0; i < N/2; i++) {
				c[i] = a[i] * b[i];
				printf("thread_id= %d \t c[%3i] = %i\n", thread_id, i, c[i]);
			}

			#pragma omp section
			for (i = 0; i < N/2; i++) {
				c[i] = a[i] * b[i];
				printf("thread_id= %d \t c[%3i] = %i\n", thread_id, i, c[i]);
			}

			#pragma omp section
			for (i = 0; i < N/2; i++) {
				c[i] = a[i] * b[i];
				printf("thread_id= %d \t c[%3i] = %i\n", thread_id, i, c[i]);
			}

			#pragma omp section
			for (i = 0; i < N/2; i++) {
				c[i] = a[i] * b[i];
				printf("thread_id= %d \t c[%3i] = %i\n", thread_id, i, c[i]);
			}

			#pragma omp section
			for (i = 0; i < N/2; i++) {
				c[i] = a[i] * b[i];
				printf("thread_id= %d \t c[%3i] = %i\n", thread_id, i, c[i]);
			}

			#pragma omp section
			for (i = 0; i < N/2; i++) {
				c[i] = a[i] * b[i];
				printf("thread_id= %d \t c[%3i] = %i\n", thread_id, i, c[i]);
			}

			#pragma omp section
			for (i = 0; i < N/2; i++) {
				c[i] = a[i] * b[i];
				printf("thread_id= %d \t c[%3i] = %i\n", thread_id, i, c[i]);
			}

			#pragma omp section
			for (i = 0; i < N/2; i++) {
				c[i] = a[i] * b[i];
				printf("thread_id= %d \t c[%3i] = %i\n", thread_id, i, c[i]);
			}

			#pragma omp section
			for (i = N/2; i < N; i++) {
				c[i] = a[i] * b[i] + 1;
				printf("thread_id= %d \t c[%3i] = %i\n", thread_id, i, c[i]);
			}
		}
	}
}
