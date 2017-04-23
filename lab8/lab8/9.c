#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) { 

	int rank;
	int nproc;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	int i;

	int a = 9;

	if(rank == 0) {

		a = 5;
		MPI_Send(&a, 1, MPI_INT, 1, 2, MPI_COMM_WORLD);
		MPI_Send(&a, 1, MPI_INT, 1, 8, MPI_COMM_WORLD);
		MPI_Send(&a, 1, MPI_INT, 1, 9, MPI_COMM_WORLD);

	}
	else 
		MPI_Recv(&a, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	if(rank == 1)
		printf("%d\n", a);
	MPI_Finalize();
	return 0;
}