#include<mpi.h>
#include<stdio.h>
#include <string.h>
/**
 * @author cristian.chilipirea
 * Run: mpirun -np 4 ./a.out
 */

int graph[][2] = { { 0, 1 }, 
                  { 1, 0 }, { 1, 2 }, { 1, 3 }, 
                  { 2, 1 }, { 2, 3 },
                  { 3, 1 }, { 3, 2 } };
int edges = 8;

const int INF = (1 << 30);


void reuneste(int* top1, int* top2, int n) {

	int i;
	for(i = 0 ; i < n ; ++i)
		if(top2[i] == -1)
			top2[i] = top1[i];
}  


int main(int argc, char * argv[]) {

	int rank;
	int n;

	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Request request;


	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n);


	int i;
	int distance = (rank == 0) ? 0 : INF;
	int parent;

	if(rank == 0) {

		parent = 0;

		for(i = 0 ; i < edges; ++i)
			if(graph[i][0] == rank)
				MPI_Send(&distance, 1, MPI_INT, graph[i][1], 111, MPI_COMM_WORLD);
	} 

	while(1) {

		int recvDistance;
		MPI_Recv(&recvDistance, 1 , MPI_INT, MPI_ANY_SOURCE, 111, MPI_COMM_WORLD, &status);

		if(distance > recvDistance + 1) {

			distance = recvDistance + 1;
			parent = status.MPI_SOURCE;

			printf("Node %d , parent: %d, distance: %d\n", rank, parent, distance);

			for(i = 0 ; i < edges; ++i)
				if(graph[i][0] == rank)
					MPI_Send(&distance, 1, MPI_INT, graph[i][1], 111, MPI_COMM_WORLD);
		}

	}
	
	MPI_Finalize();
	return 0;
}