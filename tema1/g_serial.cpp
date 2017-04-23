#include <bits/stdc++.h>
#include <omp.h>

using namespace std;

const int NMAX = 9e4;
const int NEIGHBOURS = 8;

const int dx[] = {-1, -1, -1, 0, 1, 1, 1, 0};
const int dy[] = {-1, 0, 1, 1, 1, 0, -1, -1};

int** state[2];
char* buffer;

int n; int m;

void read() {

	cin >> n >> m;

	for(int q = 0 ; q < 2; ++q) {

		state[q] = (int**) malloc(n * sizeof(int*));

		for(int i = 0 ; i < n; ++i) 
			state[q][i] = (int *) calloc(m, sizeof(int));
	}

	buffer = (char*) malloc(sizeof(char) * (m * 2 + 100));

	fgets(buffer, NMAX, stdin);

	for(int i = 0; i < n ; ++i) {

		fgets(buffer, NMAX, stdin);	

		int len = strlen(buffer);
		int cnt = 0;

		for(int j = 0 ; j < len ; ++j) {

			if(buffer[j] == '.')
				state[0][i][cnt++] = 0;

			if(buffer[j] == 'X')
				state[0][i][cnt++] = 1;
		}

		if(cnt != m) {
			cout << "Fisierul nu are numarul de coloane corespuncator\n";
			exit(0);
		}
	}
}


void print(int ind) {

	for(int i = 0 ; i < n; i++) {
		for(int j = 0 ; j < m ; ++j) 
			if(state[ind][i][j] == 1)
				cout << 'X' << ' ';
			else 
				cout << '.' << ' ';
		cout << '\n';
	}
}

void solve(int k) {
	
	for(int q = 0 ; q < k ; ++q) {

		for(int i = 0; i < n ; ++i)
			for(int j = 0; j < m ; ++j) {

				int cntalive = 0;

				if(state[0][i][j] == 1)
					for(int l = 0 ; l < NEIGHBOURS; ++l) {

						int nx = i + dx[l];
						int ny = j + dy[l];

						if(nx == n) nx = 0;
						if(ny == m) ny = 0;
						if(nx == -1) nx = n - 1;
						if(ny == -1) ny = m - 1;
						state[1][nx][ny]++;
					}
			}

		for(int i = 0; i < n ; ++i)
			for(int j = 0; j < m ; ++j) {

			int cntalive = state[1][i][j];
			state[1][i][j] = state[0][i][j];//cntalive == 2 && state == 'DEAD'
			

			if(cntalive < 2)
				state[1][i][j] = 0;
			else if(state[0][i][j] == 1 && (cntalive == 2 || cntalive == 3))
				state[1][i][j] = 1;
			else if(cntalive > 3)
				state[1][i][j] = 0;
			else if(state[0][i][j] == 0 && cntalive == 3)
				state[1][i][j] = 1;
		}

				
		for(int i = 0; i < n; ++i)
			for(int j = 0; j < m ; ++j) {
				state[0][i][j] = state[1][i][j];
				state[1][i][j] = 0;
			}
	}

	print(0);
}

int main(int argc, char* argv[]) {

	if(argc != 4) {
		cout << "Numar insuficient de argumente\n";
		return 0;
	}

	freopen(argv[1], "r", stdin);
	freopen(argv[3], "w", stdout);

	int k = atoi(argv[2]);

	read();
	solve(k);
	
	for(int q = 0 ; q < 2; ++q) {
		for(int i = 0 ; i < n ; ++i)
			free(state[q][i]);
		free(state[q]);
	}

	free(buffer);

	return 0;
}