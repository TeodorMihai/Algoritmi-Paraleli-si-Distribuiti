#include "omp.h"
#include <stdio.h>


int v[10000000];
int cv[10000000];

void swap(int* a, int* b) {

	int aux;
	aux = *a;
	*a = *b;
	*b = aux;
}

void merge(int* v, int st, int dr) {

	if(st >= dr) return ;

	int mid = (st + dr) / 2;

	#pragma omp parallel sections private(st, mid, dr)
	{
		#pragma omp section 
		{
			merge(v, st, mid);
		}

		#pragma omp section
		{
			merge(v, mid + 1, dr);
		}
	}
	
	//implicit barrier

	int a = st; int b = mid + 1; int ind = 0;

	while(a != mid + 1 || b != dr + 1) {

		if(a == mid + 1) { cv[ind++] = v[b++]; continue; }
		if(b == dr + 1)  { cv[ind++] = v[a++]; continue; }
		
		if(v[a] > v[b]) 
			cv[ind++] = v[b++];
		else 
			cv[ind++] = v[a++];
	}

	int i;

	for(i = 0 ; i < ind; ++i)
		v[i + st] = cv[i];
}

int main() {

	FILE *in = fopen("merge.in", "r");
	int n; 

	fscanf(in, "%d", &n);

	int i;
	for(i = 1 ; i <= n ; ++i)
		fscanf(in, "%d", &v[i]);

	merge(v, 1, n);

	for(i = 1; i < n ; ++i)
		if(v[i] > v[i + 1]){
			printf("no\n");
			return 0;
		}			

	printf("yes\n");

	return 0;
}