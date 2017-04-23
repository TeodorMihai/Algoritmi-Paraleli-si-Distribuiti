#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) { 

	int rank;
	int nproc;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	printf("Hrllo from %i %i\n", rank, nproc);
	
	int a = 5;
	if(rank == 0)
		MPI_Send(&a, 1, MPI_INT, 1, 2, MPI_COMM_WORLD);
	else 
		MPI_Recv(&a, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	MPI_Finalize();
	return 0;
}