#include <mpi.h>
#include <stdio.h>


char s[] = "ana\0nanan";
char rec[100];

int main(int argc, char* argv[]) {

	int rank;
	int nproc;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);


	printf("hello from %i %i\n", rank, nproc);

	if(rank == 0) {

		//int i;
		//for(i = 0 ; i < 1e4; ++i)

		MPI_Send(&s[0], sizeof(s), MPI_CHAR, 1, 111, MPI_COMM_WORLD);

		printf("Send return\n");
	}
	else {

		MPI_Recv(&rec[0], sizeof(rec) , MPI_CHAR, 0, 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}


	MPI_Finalize();

	return 0;
}