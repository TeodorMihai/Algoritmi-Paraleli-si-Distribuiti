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
	//printf("Hello from %i/%i\n", rank, n);

	int top[n];
	int sons = 0;
	int i;
	int actives[edges];
	int accept, yes = 1, no = 0;

	//no e echivalentul topoligiei nule din algoritmul clasic
	memset(actives, 0, sizeof(actives));

	for(i = 0 ; i < n; ++i)
		top[i] = -1;//topologia vida

	if(rank == 0) {

		for(i = 0 ; i < edges; ++i)
			if(graph[i][0] == rank) 
				MPI_Send(&no, 1, MPI_INT, graph[i][1], 111, MPI_COMM_WORLD);

		for(i = 0 ; i < edges; ++i)
			if(graph[i][1] == rank) {

				MPI_Recv(&accept, 1, MPI_INT, graph[i][0], 111, MPI_COMM_WORLD, &status);
				if(accept == 1) {
					top[graph[i][0]] = rank;
					sons++;
				}
			}

		printf("%d has %d sons\n", rank, sons);

	} else {

		MPI_Recv(&accept, 1, MPI_INT, MPI_ANY_SOURCE, 111, MPI_COMM_WORLD, &status);
		//printf("%d %d\n", status.MPI_SOURCE, rank);
		//primul primit e parintele, stiu parintii dar nu si fii
		top[rank] = status.MPI_SOURCE;

		//trimit catre toti ceilalti (in afara de parinte), marchez prin no tipul de mesaj(ca nu e mesaj de tipul pentru parinte)
		for(i = 0 ; i < edges; ++i)
			if(graph[i][0] == rank &&  top[rank] != graph[i][1]) 
				MPI_Send(&no, 1, MPI_INT, graph[i][1], 111, MPI_COMM_WORLD);

		//trimit catre parinte, sa-i zic ca sunt fiul lui
		MPI_Send(&yes, 1, MPI_INT, top[rank], 111, MPI_COMM_WORLD);

		//primesc de la fiecare vecin in afara de unul si anume parintele
		for(i = 0 ; i < edges; ++i)
			if(graph[i][1] == rank && top[rank] != graph[i][0]) {

				MPI_Recv(&accept, 1, MPI_INT, graph[i][0], 111, MPI_COMM_WORLD, &status);
				if(accept == 1) {
					top[graph[i][0]] = rank;
					sons++;
				}
			}
		
		printf("%d has %d sons\n", rank, sons);
	}

	//am aflat topologia, trebuie doar centralizata
	//bariera?

	MPI_Barrier(MPI_COMM_WORLD);

	if(rank == 0) {
		
		for(i = 0 ; i < sons; ++i) {
			int topRet[n];
			MPI_Recv(&topRet[0], n, MPI_INT, MPI_ANY_SOURCE, 111, MPI_COMM_WORLD, &status);
			reuneste(topRet, top, n);	
		}
		//sends back

		for(i = 0 ; i < n; ++i)
			if(top[i] == rank)
				MPI_Send(top, n, MPI_INT, i , 111, MPI_COMM_WORLD);

	} else {

		for(i = 0 ; i < sons; ++i) {
			int topRet[n];
			MPI_Recv(topRet, n, MPI_INT, MPI_ANY_SOURCE, 111, MPI_COMM_WORLD, &status);
			reuneste(topRet, top, n);	
		}

		MPI_Send(top, n, MPI_INT, top[rank], 111, MPI_COMM_WORLD);
		MPI_Recv(top, n, MPI_INT, MPI_ANY_SOURCE, 111, MPI_COMM_WORLD, &status);

		for(i = 0 ; i < n; ++i)
			if(top[i] == rank)
				MPI_Send(top, n, MPI_INT, i, 111, MPI_COMM_WORLD);
	}


	for(i = 0 ; i < n ; ++i)
		printf("%d ", top[i]);
	printf("\n");

	
	MPI_Finalize();
	return 0;
}