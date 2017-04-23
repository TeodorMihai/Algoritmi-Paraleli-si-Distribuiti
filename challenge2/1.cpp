#include <bits/stdc++.h>
#include <omp.h>

using namespace std;

const int TRIESMAX = (1 << 14);
const int NMAX = 128;

int v[NMAX];
int corect[NMAX];
int randV[TRIESMAX][NMAX];
int sorted[NMAX];
int indexSorted = -1;

int nrthreads[NMAX * NMAX];

int N;

int compare(int *v1, int *v2) {

	for(int i = 0 ; i < N; ++i)
		if(v1[i] != v2[i]) return 0;

	return 1;
}

int main() {

	cout << "Introduceti dimensiunea vectorului, trebuie sa fie putere a lui 2.\n";
	cin >> N;

	srand(time(NULL));

	for(int i = 0; i < N; i++) {
		v[i] = rand();
		corect[i] = v[i];
	}

	sort(corect, corect + N);

	omp_set_num_threads(30000);

	#pragma omp parallel
	{
			
		int num = omp_get_num_threads();
		int id = omp_get_thread_num();

		int tries = num / N;

		if(id == 0)
			cout << "Incercari:"<< tries << '\n';
		
		randV[id / N][id % N] = -1;
		
		int pos = rand() % N;
		//pozitia pentru elementul id % N din incercarea a id / N
		//este scriere concurenta dar nu conteaza, daca se intampla sa fie
		//o scriere concurenta cf principiului cutiei, nu o sa fie asignate
		//pozitii pentru toate elementele deci nu se va lua in considerare soratarea
		randV[id / N][pos] = v[id % N];

		#pragma omp barrier

		if(randV[id / N][id % N] == -1) 
			sorted[id / N] = 1;

		#pragma omp barrier

		if(pos + 1 < N && randV[id / N][pos] > randV[id / N][pos + 1])
			sorted[id / N] = 1;

		#pragma omp barrier

		if(id < tries * N && sorted[id / N] == 0)
			indexSorted = id / N;
	}


	if(indexSorted == -1) {
		cout << "Prea putine threaduri petru a reusi sortarea\n";
	} else if(compare(randV[indexSorted], corect) == 1)
		cout << "Sortare reusita in O(1)\n";
	else cout << "Something is wrong\n";

	
	return 0;

}