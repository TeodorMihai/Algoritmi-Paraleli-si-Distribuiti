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

	int a;

	if(rank == 0)
		a = 0;

	if(rank == 0) {
		a += 2;
		MPI_Send(&a, 1, MPI_INT, (rank + 1) % nproc, 2, MPI_COMM_WORLD);

		MPI_Recv(&a, 1, MPI_INT, (rank - 1 + nproc) % nproc, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	} else  {

		MPI_Recv(&a, 1, MPI_INT, (rank - 1 + nproc) % nproc, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		a += 2;
		MPI_Send(&a, 1, MPI_INT, (rank + 1) % nproc, 2, MPI_COMM_WORLD);
	}
	
	if(rank == 0)
		printf("%d\n", a);

	MPI_Finalize();
	return 0;
}