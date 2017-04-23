#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) { 

	int rank;
	int nproc;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	int i;

	int a = 5;
	int b = 2;
	int c = 1;

	if(rank == 0 || rank == 1)
		MPI_Send(&a, 1, MPI_INT, 2, 2, MPI_COMM_WORLD);
	else {
		MPI_Recv(&b, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&c, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	if(rank == 2) {
		printf("%d\n", b);
		printf("%d\n", c);
	}

	MPI_Finalize();
	return 0;
}