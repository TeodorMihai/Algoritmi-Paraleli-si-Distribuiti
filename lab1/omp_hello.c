/******************************************************************************
* FILENAME: omp_hello.c
* DESCRIPTION: Hello world program for OpenMP
* AUTHOR: Cristian Chilipirea
* DATE:	10-October-2016
******************************************************************************/
#include <omp.h>
#include <stdio.h>

int main (int argc, char ** argv)  {
	int thread_id;

	#pragma omp parallel private(thread_id)
	{
		/* 
			The threads are started; 
			The next code region is executed once for each thread;
			The default number of threads is equal to the number of CPU cores;
			A variable named thread_id is allocated for each thread.
		*/

		thread_id = omp_get_thread_num(); // Get the ID of each thread
		/* 
			For thread with ID 0 this message always appears before the Number of threads;
			This is not true for the thread with ID 1. Why?
		*/
		printf("Hello World from thread = %d\n", thread_id);

		#pragma omp master
		{
			/* 
				Only the master thread executes the next code region;
				The master thread is the main execution thread of the program;
				The master thread starts all the other threads and continues execution after all other threads exit.
			*/

			int number_of_threads;
			number_of_threads = omp_get_num_threads(); // Get the total number of threads
			printf("Number of threads = %d\n", number_of_threads);
		}
	}  // All threads exit and join the master thread
	return 0;
}