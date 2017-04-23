#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) { 

	int rank;
	int nproc;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	int v[100];
	int r[25];
	int i;
	if(rank == 0)
		for(i = 0 ; i < 100; ++i)
			v[i] = i;


	MPI_Scatter(&v[0], 25 , MPI_INT, &r[0], 25, MPI_INT, 0, MPI_COMM_WORLD);

	if(rank == 1) {
		for(i = 0 ; i < 25; ++i)
			printf("%d ", r[i]);

		printf("\n");
	}

	for(i = 0; i < 25; ++i)
		r[i] += 42;

	MPI_Gather(&r[0] , 25, MPI_INT, &v[0], 25, MPI_INT, 0, MPI_COMM_WORLD);


	if(rank == 0)
		for(i = 0 ; i < 100; ++i)
			printf("%d ", v[i]);

	MPI_Finalize();


	return 0;
}