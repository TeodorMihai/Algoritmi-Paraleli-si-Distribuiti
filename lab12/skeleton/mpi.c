#include <mpi.h>
#include <stdio.h>
#include <math.h>

long long mini(long long a, long long b) {
	return a < b ? a : b;
}

int main(int argc, char* argv[]) { 

	int rank;
	int nproc;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	//printf("Hello from %i %i\n", rank, nproc);
	long long i;
	long long x;

	if(rank == 0) {

		scanf("%lld", &x);

		for(i = 1; i < nproc; ++i)
			MPI_Send(&x, 1, MPI_LONG, i, 111, MPI_COMM_WORLD);
	} else {

		MPI_Recv(&x, 1, MPI_LONG, 0, 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	}

	long long sx = (long long) sqrt((double)x) + 1;

	long long block = x / nproc;
	long long left = rank * block;
	long long right = mini((rank + 1) * block, x);


	//printf("left: %lld right: %lld \n ", left, right);
	int nr_div = 0;
	for(i = left; i < right; ++i)
		if(i >= 2 && x % i == 0) {
			nr_div++;
		}

	if(rank != 0) 
		MPI_Send(&nr_div, 1, MPI_INT, 0, 111, MPI_COMM_WORLD);
	else {

		int recva;
		for(i = 1 ; i < nproc; ++i) {
			MPI_Recv(&recva, 1, MPI_INT, i, 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			nr_div += recva;
		}
	}

	if(rank == 0) {
		printf("%d\n", nr_div);
	}

	MPI_Finalize();
	return 0;
}


