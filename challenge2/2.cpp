
#include <bits/stdc++.h>
#include <omp.h>

using namespace std;

const int NMAX = 1e5;

int N;

struct element {

	int ind;
	int a;
	element() { }
	element(int _ind, int _a) : ind(_ind), a(_a) { }
};

element v[NMAX];


int bs(element v[], int left, int right, element el) {

	int step, pos;

	for( step = 1; step < N; step <<= 1);

	for(pos = left ; step ; step >>= 1)
		if(pos + step < right && 
			(( v[pos + step].a < el.a)  || (v[pos + step].a == el.a && v[pos + step].ind < el.ind) ))
			pos += step;

	if(v[pos].a < el.a || (v[pos].a == el.a && v[pos].ind < el.ind))
		return pos + 1;
	return pos;		
}

void merge(element A[], int Left, int Right, int End, element B[], int id) {
	

	//enters here End - Left threads Lefft <= id < End
	//[Left, Right), [Right, End)

	if(id < Right) {

		int lower2 = bs(A , Right, End, A[id]) - Right;
		int lower1 = id - Left;
		B[Left + lower1 + lower2] = A[id];

	} else {

		int lower1 = bs(A , Left, Right, A[id]) - Left;
		int lower2 = id - Right;
		B[Left + lower1 + lower2] = A[id];
	}
}

void compareVectors(element * a, int * b) {
	// DO NOT MODIFY
	int i;
	for(i = 0; i < N; i++) {
		if(a[i].a != b[i]) {
			printf("Sorted incorrectly\n");
			return;
		}
	}
	printf("Sorted correctly\n");
}

void displayVector(int * v) {
	// DO NOT MODIFY
	int i;
	int displayWidth = 2 + log10(v[N-1]);
	for(i = 0; i < N; i++) {
		printf("%*i", displayWidth, v[i]);
	}
	printf("\n");
}

int cmp(const void *a, const void *b) {
	// DO NOT MODIFY
	int A = *(int*)a;
	int B = *(int*)b;
	return A-B;
}

int main(int argc, char *argv[]) {

	cout << "Introduceti dimensiunea vectorului, trebuie sa fie putere a lui 2.\n";
	cin >> N;

	element *v = (element*)malloc(sizeof(element)*N);
	int *vQSort = (int*)malloc(sizeof(int)*N);
	element *vNew = (element*)malloc(sizeof(element)*N);
	element *aux;
	int i, width;
	// generate the vector v with random values
	// DO NOT MODIFY
	srand(time(NULL));
	for(i = 0; i < N; i++) {
		v[i].ind = i;
		v[i].a = rand() % N;
	}


	// make copy to check it against qsort
	// DO NOT MODIFY
	for(i = 0; i < N; i++)
		vQSort[i] = v[i].a;
	qsort(vQSort, N, sizeof(int), cmp);


	// sort the vector v
	#pragma omp parallel private(i, width)
	{
		int num = omp_get_num_threads();
		int id = omp_get_thread_num();

		if(num != N)
			exit(0);

		for (width = 1; width < N; width = 2 * width) {

			for (i = 0; i < N; i = i + 2 * width) 
				if(i <= id && id < i + 2 * width)
					merge(v, i, i+width, i + 2 * width, vNew, id);

			#pragma omp barrier
			
			if(id == 0) {
				aux = v;
				v= vNew;
				vNew=aux;
			}

			#pragma omp barrier
		}
	}


	//displayVector(vQSort);
	compareVectors(v, vQSort);

	return 0;
}