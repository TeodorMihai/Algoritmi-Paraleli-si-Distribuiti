#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) { 

	int rank;
	int nproc;

	int v[100];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	int i;
	if(rank == 0)
		for(i = 0 ; i < 100; ++i)
			v[i] = i;

	int a = 5;
	if(rank == 0)
		MPI_Send(&v[0], 100, MPI_INT, 1, 2, MPI_COMM_WORLD);
	else 
		MPI_Recv(&v[0], 100, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	for(i = 0 ; i < 100; ++i)
		if(rank == 1)
			printf("%d ", v[i]);

	MPI_Finalize();
	return 0;
}