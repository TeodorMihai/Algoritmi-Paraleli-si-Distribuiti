#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) { 

	int rank;
	int nproc;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	int v[100];
	int i;
	if(rank == 0)
		for(i = 0 ; i < 100; ++i)
			v[i] = i;

	MPI_Bcast(&v[0], 100, MPI_INT, 0, MPI_COMM_WORLD);

	for(i = 0 ; i < 100; ++i)
		if(rank == 1)
			printf("%d ", v[i]);

	printf("\n");

	for(i = 0 ; i < 100; ++i)
		if(rank == 2)
			printf("%d ", v[i]);


	MPI_Finalize();


	return 0;
}