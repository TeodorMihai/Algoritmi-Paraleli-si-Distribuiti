#include<mpi.h>
#include<stdio.h>
#include <string.h>
#include <stdlib.h>

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


		int stop = 0;
		MPI_Send(&stop, 1, MPI_INT, 4, 222, MPI_COMM_WORLD);
		parent = 0;

		for(i = 0 ; i < edges; ++i)
			if(graph[i][0] == rank)
				MPI_Send(&distance, 1, MPI_INT, graph[i][1], 111, MPI_COMM_WORLD);

		stop = 1;
		MPI_Send(&stop, 1, MPI_INT, 4, 222, MPI_COMM_WORLD);

	}

	int stop = 0; 

	if(rank != 4) {
		while(1) {

			int recvDistance;
			MPI_Request req = MPI_REQUEST_NULL;
			MPI_Irecv(&recvDistance, 1 , MPI_INT, MPI_ANY_SOURCE, 111, MPI_COMM_WORLD, &req);

			int flg = 0;
			while(flg == 0) {
				
				MPI_Test(&req, &flg, &status);

				if(flg == 0) break;

				int flgtoken = 0;
				int token;
				MPI_Request reqToken = MPI_REQUEST_NULL;
				MPI_Status statusToken;

				MPI_Irecv(&token, 1 , MPI_INT, 4, 222, MPI_COMM_WORLD, &reqToken);
				MPI_Test(&reqToken, &flgtoken, &statusToken);
				if(flgtoken == 1) {//am primit

					if(token == 0) { //question?, sends back answer
						MPI_Send(&stop, 1, MPI_INT, 4, 222, MPI_COMM_WORLD);
					} else if(token == 3) {
						MPI_Finalize();
						return 0;
					}
				}

			}

			MPI_Send(&stop, 1, MPI_INT, 4, 222, MPI_COMM_WORLD);

			//MPI_Wait(&req, &status);

			if(distance > recvDistance + 1) {

				distance = recvDistance + 1;
				parent = status.MPI_SOURCE;

				printf("Node %d , parent: %d, distance: %d\n", rank, parent, distance);

				for(i = 0 ; i < edges; ++i)
					if(graph[i][0] == rank)
						MPI_Send(&distance, 1, MPI_INT, graph[i][1], 111, MPI_COMM_WORLD);
			}

			stop = 1;//stopping
			MPI_Send(&stop, 1, MPI_INT, 4, 222, MPI_COMM_WORLD);

		}
	}

	if(rank == 4) {

		int state[4];
		for(i = 0 ; i < 4; ++i)
			state[i] = 1; //initial sunt oprite
		int ss = 0;
		while(1) {

			MPI_Recv(&ss, 1, MPI_INT, MPI_ANY_SOURCE, 222, MPI_COMM_WORLD, &status);

			state[status.MPI_SOURCE] = ss;
			int tryFinish = 1;

			for(i = 0 ; i < 4; ++i)
				if(state[i] == 0) 
					tryFinish = 0;

			if(tryFinish == 0) continue; 

			for(i = 0 ; i < 4; ++i) {
				int what = 3;
				MPI_Send(&what, 1, MPI_INT, i, 222, MPI_COMM_WORLD);
			}

			MPI_Finalize();
			return 0;

			printf("One try\n");

			for(i = 0 ; i < 4; ++i){
				int question = 0;
				MPI_Send(&question, 1, MPI_INT, i, 222, MPI_COMM_WORLD);
			}

			int ok = 1;

			for(i = 0 ; i < 4; ++i) {
				MPI_Recv(&ss, 1, MPI_INT, i, 222, MPI_COMM_WORLD, &status);
				state[i] = ss;

				if(ss == 0) ok = 0;
			}
			if(ok == 1) {
				ss = 3;
				for(i = 0 ; i < 4; ++i) 
					MPI_Send(&ss, 1, MPI_INT, i, 222, MPI_COMM_WORLD);
				MPI_Finalize();
				return 0;
			}
			//trimite mesaj de oprire
		}
	}
	
	MPI_Finalize();
	return 0;
}