#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) { 

	int rank;
	int nproc;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	
	int a ;

	if(rank == 0)
		a = 5;

	MPI_Bcast(&a, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if(rank != 0)
		printf("%d ", a);

	MPI_Finalize();


	return 0;
}