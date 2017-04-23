#include<string.h>
#include<stdio.h>
#include<time.h>

/**
 * @author cristian.chilipirea
 * 
 */

int n1GB = 1024*1024*1024;
char v[1024*1024*1024];

int main(int argc, char * argv[]) {
	int c;
	memset(v, '1', n1GB);

	clock_t begin = clock();

	int i;

	omp_set_num_threads(2);
	#pragma omp parallel for private(i)
	for(i=0;i<32;i++)
		memset(v+(i*32*1024*1024), '1', 32*1024*1024);


	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("%f\n",time_spent);
	return 0;
}