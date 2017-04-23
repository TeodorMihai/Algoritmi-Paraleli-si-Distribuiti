#include <mpi.h>
#include <stdio.h>

const int NMAX = 100;

int main(int argc, char* argv[]) {

	int rank; int nproc;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	printf("hello from %i %i\n", rank, nproc);

	int i;
	int v[NMAX];

	v[0] = 0;
	v[++v[0]] = rank;

	int next = (rank + 1) % nproc;
	int prev = (rank - 1) % nproc;
	int leader = -1;

	for(i = 0 ; i < nproc - 1; ++i) {

		MPI_Send(&v[v[0]], 1, MPI_INT, next , 111, MPI_COMM_WORLD);
		MPI_Recv(&v[++v[0]], 1, MPI_INT, prev , 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	for(i = 1 ; i <= v[0]; ++i)
		if(leader < v[i])
			leader = v[i];

	printf("%d %d\n",rank, leader);

	MPI_Finalize();

	return 0;
}