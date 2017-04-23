#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) { 


	srand(time(NULL));

	int l = 1.0;


	int rank;
	int nproc;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	
	int i;

	int in = 0;
	int out = 0;


	for(i = 0 ; i < 1000000000; ++i) {


		double x = (rand()  % 100) / 100.0;
		double y = (rand()  % 100) / 100.0;

		if( (x - 0.5) * (x - 0.5) + (y - 0.5) * (y - 0.5) <= 0.5 * 0.5) //in circle
			in++;
		else
			out++;
	}


	//aria cerc = pi * r^2
	//patrat = 4 * r * r
	//pi = cerc / patrat * 4

	double rez =  1.0 * in / (in + out) * 4.0 ;
	double res[nproc];


	if(rank != 0)
		MPI_Send(&rez, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
	else {

		for(i = 1; i < nproc; ++i)
			MPI_Recv(&res[i], 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		res[0] = rez;
		rez = 0;
		for(i = 0 ; i < nproc; ++i)
			rez += res[i];
		rez /= 1.0 * nproc;
		printf("%lf\n", rez);
	}


	MPI_Finalize();
	return 0;
}