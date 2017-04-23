#include <bits/stdc++.h>
#include "omp.h"
#include <unistd.h>

using namespace std;

int main() {

	int mini = 0;
	for(int j = 1; j < 1e7; ++j) {

		int temp, count, i = 0;
		temp = count = 0;

		omp_set_num_threads(3);
		int nrth = 0;
		#pragma omp parallel 
		{	
			nrth = omp_get_num_threads();
			for(i = 0; i < 100; i++) {
				//sleep(0.0001);
				count++;
			}
		}

		if(mini < count && nrth == 3)
			mini = count;
	}
	cout << mini << '\n';

	return 0;
}