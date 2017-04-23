/******************************************************************************
* FILENAME: omp_reduction.c
* DESCRIPTION: OpenMP program that demonstrates for work-sharing with reduction
* AUTHOR: Cristian Chilipirea
* DATE:	10-October-2016
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#define N 20

int main (int argc, char** argv) {
	int i, sum;

	sum = 10000000; // Why does this work?

	#pragma omp parallel for reduction(+:sum) // What is the initial value of sum?
	for (i = 0; i <= N; i++)
		sum = sum + i;

	printf("Sum is %i \n", sum);
	printf("Sum also is %i \n", (N + 1) * (N / 2));
}
