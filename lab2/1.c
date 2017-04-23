/******************************************************************************
* FILENAME: omp_bug_for.c
* DESCRIPTION: OpenMP program that contains bug caused by for directive
* AUTHOR: Cristian Chilipirea
* DATE:	10-October-2016
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#define N 20

int main (int argc, char** argv) {
	int i, thread_id;
	int a[N], b[N], c[N];

	for (i = 1; i < N; i++) {
		a[i] = i * 10;
		b[i] = i;
	}

	#pragma omp parallel for shared(a, b, c) private(thread_id)
	for (i = 1; i < N; i++)	{
		c[i] = a[i] / b[i];
		thread_id = omp_get_thread_num();
		printf("thread_id = %i \t c[%3i] = %i\n", thread_id, i, c[i]);
	}
	
	return 0;
}
