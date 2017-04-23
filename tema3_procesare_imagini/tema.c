#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LMAX 1000
#define MAXFILENAME 30


int n; int rank;
char buffer[LMAX];
int neigh[LMAX];
int* top = NULL;
int sons = 0;

typedef struct Task {
	char effect[MAXFILENAME];
	char input[MAXFILENAME];
	char output[MAXFILENAME];
} Task;


typedef struct Myimage {

	int n; int m; int maxValue;
	char type[100];
	char comment[400];
	int** a;
} Myimage;

typedef struct Filter {

	int sum;
	int mat[3][3];
	int dep;
} Filter;


Filter blur = { 
	.sum = 16, 
	.mat = {
		{1, 2, 1},
		{2, 4, 2},
		{1, 2, 1}
	},
	.dep = 0
};

Filter smooth = {
	.sum = 9,
	.mat = {
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1}
	},
	.dep = 0
};

Filter sharpen = {

	.sum = 3,
	.mat = {
		{0, -2, 0},
		{-2, 11, -2},
		{0, -2, 0}
	},
	.dep = 0
};

Filter meanremove = {
	.sum = 1,
	.mat = {
		{-1, -1, -1},
		{-1, 9, -1},
		{-1, -1, -1}
	},
	.dep = 0
};


void checkArguments(int , char**);
void initProcesses(int, char**);
void readTopology(char**);
void parse(char*);
int getNext(char*, int*);
void waveTree();
void unite(int*, int*, int);
void printNeighbors();
Task* readImages(char**, int*);
Task parseImagesFile(char*);
void readImage(char*, Myimage*);
void writeImage(char* , Myimage*);
void solveTasksRoot(Task*, int, char** );
void sendBucket(char* , int, int, int**);
void receiveBucket(char*, int*, int*, int***);
void printToplogy();
void receiveComp(int, int, int**);
void sendComp(int, int, int**);


void checkArguments(int argc, char* argv[]) {

	if(argc != 4) {
		fprintf(stderr, "Utilizare: mpirun -np N ./a.out topologie.in imagini.in statistica.out\n");
		exit(0);
	}
}

void initProcesses(int argc, char** argv) {

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &n);
}

int getNext(char* buffer, int* i) {

	int nr = 0;

	for( ; buffer[*i] != 0 && !(buffer[*i] >= '0' && buffer[*i] <= '9') ; ++*i);	

	int findNumber = 0;

	for( ; buffer[*i] >= '0' && buffer[*i] <= '9'; ++*i)
		nr = nr * 10 + buffer[*i] - '0', findNumber = 1;

	if(findNumber == 1)
		return nr;
	else 
		return -1;
}

void parse(char* buffer) {	

	
	int i = 0;
	int nr = 0;

	for(i = 0 ; buffer[i] != 0 && buffer[i] != ':'; ++i)
		nr = nr * 10 + buffer[i] - '0';
	
	++i;
	int x = -1;

	do {
	
		x = getNext(buffer, &i);

		if(x != -1)
			neigh[++neigh[0]] = x;

	} while(x != -1);
}

void readTopology(char* argv[]) {

	FILE *top = fopen(argv[1], "r");

	if(top == NULL) {
		perror("Cannot open topology file.");
		MPI_Finalize();
		exit(0);
	}

	int nrLines = 0;
	
	while(fgets(buffer, LMAX, top) != NULL) nrLines++;

	if(nrLines > n) {
		fprintf(stderr, "More lines than processes.\n");
		MPI_Finalize();
		exit(0);
	}

	if(rank > nrLines) { 
		MPI_Finalize();
		exit(0);
	}

	int line = 0;

	rewind(top);

	while(fgets(buffer, LMAX, top) != NULL) {

		if(line == rank) {
			parse(buffer);
			break;
		} 

		line++;
	}

	fclose(top);
}

void printNeighbors() {

	int i;

	printf("Neighbors of %d: ", rank);
	for( i = 1 ; i <= neigh[0] ; ++i)
		printf("%d ", neigh[i]);

	printf("\n");
}

void unite(int* top1, int* top2, int n) {

	int i;

	for(i = 0 ; i < n ; ++i)
		if(top2[i] == -1)
			top2[i] = top1[i];
}  

void waveTree() {

	int i;
	int accept, ecou = 1, sondaj = 0;

	MPI_Status status;

	//no e echivalentul topoligiei nule din algoritmul clasic
	top = (int*) malloc(sizeof(int) * n);
	for(i = 0 ; i < n; ++i)
		top[i] = -1;//topologia vida

	if(rank == 0) {

		//procesul initializator, trimite la toti vecinii sai mesaje de sondaj
		for(i = 1 ; i <= neigh[0]; ++i)
			MPI_Send(&sondaj, 1, MPI_INT, neigh[i], 111, MPI_COMM_WORLD);

		//primesc ecourile & alte sondaje
		for(i = 1; i <= neigh[0] ; ++i) {
			MPI_Recv(&accept, 1, MPI_INT, neigh[i], 111, MPI_COMM_WORLD, &status);
				if(accept == ecou) {
					top[neigh[i]] = rank;
					sons++;
					//ecout => tatal lui neigh[i] este rank
				}
		}

	} else {

		MPI_Recv(&accept, 1, MPI_INT, MPI_ANY_SOURCE, 111, MPI_COMM_WORLD, &status);
		//printf("%d %d\n", status.MPI_SOURCE, rank);
		//primul primit e parintele, poate fi numai sondaj
		if(accept == ecou) {
			fprintf(stderr, "Am primit mesaj de ecou in loc de sondaj\n");
			MPI_Finalize();
			exit(0);
		}

		top[rank] = status.MPI_SOURCE;

		//trimit catre toti ceilalti (in afara de parinte) mesaje de sondaj
		for(i = 1; i <= neigh[0]; ++i)
			if(neigh[i] != top[rank]) 
				MPI_Send(&sondaj, 1, MPI_INT, neigh[i], 111, MPI_COMM_WORLD);

		//trimit catre parinte ecou
		MPI_Send(&ecou, 1, MPI_INT, top[rank], 111, MPI_COMM_WORLD);

		//primesc de la fiecare vecin in afara de parinte
		//primesc si sondaje si ecouri, ma ocup doar de ocuri
		for(i = 1; i <= neigh[0]; ++i)
			if(top[rank] != neigh[i]) {

				MPI_Recv(&accept, 1, MPI_INT, neigh[i], 111, MPI_COMM_WORLD, &status);
				if(accept == ecou) {
					top[neigh[i]] = rank;
					sons++;
				}
			}
		}

	//am aflat topologia, trebuie doar centralizata
	//bariera?

	MPI_Barrier(MPI_COMM_WORLD);

	if(rank == 0) {
		
		for(i = 0 ; i < sons; ++i) {
			int topRet[n];
			MPI_Recv(&topRet[0], n, MPI_INT, MPI_ANY_SOURCE, 111, MPI_COMM_WORLD, &status);
			unite(topRet, top, n);	
		}
		//sends back

		for(i = 0 ; i < n; ++i)
			if(top[i] == rank)
				MPI_Send(top, n, MPI_INT, i , 111, MPI_COMM_WORLD);

	} else {


		//primesc de la fii toplogii, le combin, frunzele au 0 fii, nu primesc nimic
		for(i = 0 ; i < sons; ++i) {
			int topRet[n];
			MPI_Recv(topRet, n, MPI_INT, MPI_ANY_SOURCE, 111, MPI_COMM_WORLD, &status);
			unite(topRet, top, n);	
		}

		//trimit la parinte topologia combinata
		MPI_Send(top, n, MPI_INT, top[rank], 111, MPI_COMM_WORLD);
		//astept de la parinte toplogia finala
		MPI_Recv(top, n, MPI_INT, MPI_ANY_SOURCE, 111, MPI_COMM_WORLD, &status);

		//trimit mai departe topologia finala la toti fii, frunzele nu mai trimit la nimeni
		for(i = 0 ; i < n; ++i)
			if(top[i] == rank)
				MPI_Send(top, n, MPI_INT, i, 111, MPI_COMM_WORLD);
	}
}

void printToplogy() {


	int i;
	printf("top of %d:", rank);
	for(i = 0 ; i < n; ++i)
		printf("%d ", top[i]);

	printf("\n");
}

char* getNextString(char* buffer, int* i) {

	char* s = calloc(30, sizeof(char));;
	int cur = 0;

	for( ; buffer[*i] != 0 && buffer[*i] == ' '; ++*i );

	for( ; buffer[*i] != 0 && buffer[*i] != ' ' && buffer[*i] != '\n'; ++*i) {
		
		s[cur++] = buffer[*i];

		if(cur == MAXFILENAME ) {

			fprintf(stderr, "Lungimea fieserelor prea mare\n");
			MPI_Finalize();
			exit(0);
		}
	}

	s[cur++] = 0;

	return s;
}

Task parseImagesFile(char* buffer) {

	Task t;
	
	int i = 0;
	char* p =  getNextString(buffer, &i);

	strncpy(t.effect, p,  MAXFILENAME);
	free(p);

	p =  getNextString(buffer, &i);
	strncpy(t.input, p,  MAXFILENAME);
	free(p);

	p =  getNextString(buffer, &i);
	strncpy(t.output, p,  MAXFILENAME);
	free(p);

	return t;
}

Task* readImages(char *argv[], int* nrTasks) {
	
	int nrImages = 0;
	FILE* images = fopen(argv[2], "r");

	if(images == NULL) {
		perror("Cannot open images file\n");
		MPI_Finalize();
		exit(0);
	}

	fscanf(images,"%d", &nrImages);
	*nrTasks = nrImages;

	fgets(buffer, LMAX, images); //read line with the initial number
	int line = 0;

	Task* tasks = (Task*) malloc(sizeof(Task) * nrImages);
	int cur = 0;
	
	while(fgets(buffer, LMAX, images) != NULL) {
		line++;
		tasks[cur++] = parseImagesFile(buffer);
		if(line == nrImages) break;
	}

	fclose(images);

	return tasks;
}

void readImage(char* imgName, Myimage* myimg) {

	FILE *img = fopen(imgName, "r");

	char s[600];

	fgets(myimg->type, 100, img);
	fgets(myimg->comment, 400, img);

	fgets(s, 600, img);

	int i = 0;
	int m = getNext(s, &i);
	int n = getNext(s, &i);

	myimg->n = n;
	myimg->m = m;

	fgets(s, 600, img); i = 0;
	myimg->maxValue = getNext(s, &i);

	myimg->a = (int**) calloc( n + 2, sizeof(int*));

	for( i = 0 ; i <= n + 1; ++i)
		*(myimg->a + i)= (int*) calloc( m + 2, sizeof(int));

	i = 1;
	int j = 1;

	while(fgets(s, 600, img) != NULL) {

		int ind = 0 ;
		myimg->a[i][j] = getNext(s, &ind);
		++j;
		if(j == m + 1) {
			i++;
			j = 1;
		}
	}

	fclose(img);
}

void readPrint(char* fileName) {

	FILE* img = fopen(fileName, "r");
	FILE* out = fopen("1out.pgm", "w");
	char s[100];

	while(fgets(s, 100, img) != NULL) {

		fprintf(out, "%s", s);
	}
}

void deallocate(int nn, int mm, int** a) {

	int i;
	for(i = 0 ; i < nn; ++i)
		free(a[i]);
	free(a);
}

void writeImage(char* fileName, Myimage* im) {

	FILE* out = fopen(fileName, "w");

	fprintf(out, "%s", im->type);
	fprintf(out, "%s", im->comment);
	fprintf(out, "%d %d\n", im->m, im->n);
	fprintf(out, "%d\n", im->maxValue);

	int i, j;
	
	for(i = 1; i <= im->n; ++i)
		for(j = 1; j <= im->m ;++j)
			fprintf(out, "%d\n", im->a[i][j]);

	fclose(out);
}


void writeStat(char* fileName, int* stat) {

	FILE* st = fopen(fileName, "w");

	if(st == NULL) {

		perror("Cannot open stat file.\n");
		MPI_Finalize();
		exit(0);
	}

	int i;
	for(i = 0 ; i < n - 1 ;++i)
		fprintf(st, "%d: %d\n",i, stat[i]);

	fprintf(st, "%d: %d", i, stat[i]);

	fclose(st);
}


void mergee(int* auxStat, int* stat) {

	int i;
	for(i = 0 ; i < n ; ++i)
		if(auxStat[i] != 0)
			stat[i] = auxStat[i];
}


void sendBucket(char* filterType, int nn, int mm, int** a) {

	int i, j, k;

	if(sons == 0) return ;
		
	if(sons <= nn) {

	 	int dim = nn / sons + 2;
		int start = 1;
		int cnt = 0;

		for( i = 0 ; i < n; ++i)
			if(top[i] == rank) {

				int cntLine = mm + 2;
				int end = start - 1 + dim;
				cnt++;

				if(cnt == sons) end = nn + 2;

				int len = strlen(filterType);
			
				//printf("%s %d\n", filterType, rank);
				//printf("s:%d %d %s\n", len, i, filterType);

				MPI_Ssend(&len, 1, MPI_INT, i , 111, MPI_COMM_WORLD);
				MPI_Ssend(&filterType[0] , strlen(filterType) + 1, MPI_CHAR, i , 111, MPI_COMM_WORLD);
				
				int lineSend = end - (start - 1);	
				MPI_Ssend(&lineSend, 1, MPI_INT, i, 111, MPI_COMM_WORLD);//cate linii voi primi
				MPI_Ssend(&cntLine, 1, MPI_INT, i, 111, MPI_COMM_WORLD);//cat elemente are o linie

				for(j = start - 1; j < end; j++) 
					for(k = 0 ; k < cntLine; ++k)
						MPI_Ssend(&a[j][k], 1, MPI_INT, i, 111, MPI_COMM_WORLD);

				start += dim - 2;
			}
	} else {

		int cur = 1;
		for(i = 0 ; i < n ; ++i)
			if(top[i] == rank) {

				int dim = 3;
				if(cur >= nn + 1)
					dim = 0;


				int cntLine = mm + 2;
				int len = strlen(filterType);

				MPI_Send(&len, 1, MPI_INT, i , 111, MPI_COMM_WORLD);			
				MPI_Send(&filterType[0], strlen(filterType) + 1, MPI_CHAR, i , 111, MPI_COMM_WORLD);
				
				MPI_Send(&dim, 1, MPI_INT, i, 111, MPI_COMM_WORLD);//cate linii voi primi
				MPI_Send(&cntLine, 1, MPI_INT, i, 111, MPI_COMM_WORLD);//cat are o linie

				if(dim == 3)
					for(j = cur - 1; j <= cur + 1; ++j)
						for(k = 0 ; k < cntLine; ++k)
							MPI_Send(&a[j][k], 1, MPI_INT, i, 111, MPI_COMM_WORLD);
				cur++;
			}

	}
}

void receiveBucket(char* filterType, int* nn, int* mm, int*** a) {

	int i, j;
	int len;

	MPI_Recv(&len, 1 , MPI_INT, top[rank], 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(&filterType[0], len + 1, MPI_CHAR, top[rank], 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	MPI_Recv(nn, 1, MPI_INT, top[rank], 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(mm, 1, MPI_INT, top[rank], 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	if(nn == 0) return ; //nothing to receive

	*a = (int**) calloc(*nn, sizeof(int*));

	if(*a == NULL) {

		fprintf(stderr, "Can't allocate memory.\n");
		MPI_Finalize();
		exit(0);
	}

	for(i = 0 ; i < *nn; ++i) {
		(*a)[i] = (int*) calloc(*mm, sizeof(int));

		if((*a)[i] == NULL) {
			fprintf(stderr, "Can't allocate memory.\n");
			MPI_Finalize();
			exit(0);
		}
	}

	for(i = 0 ; i < *nn; ++i)
		for(j = 0 ; j < *mm; ++j)
			MPI_Recv(&(*a)[i][j], 1, MPI_INT, top[rank], 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void receiveComp(int nn, int mm, int** a) {

	int i, j, k;

	int cur = 1;
	int recvLines;

	if(sons <= nn) {
		for(i = 0 ; i < n; ++i)
			if(top[i] == rank) {

				MPI_Recv(&recvLines, 1, MPI_INT, i, 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


				for(j = cur; j < cur + recvLines; ++j)
					for(k = 1; k <= mm; ++k)
						MPI_Recv(&a[j][k], 1, MPI_INT, i, 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				
				cur += recvLines;
			}
	} else {


		for(i = 0 ; i < n; ++i)
			if(top[i] == rank) {

				MPI_Recv(&recvLines, 1, MPI_INT, i, 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


				for(j = cur; j < cur + recvLines; ++j)
					for(k = 1; k <= mm; ++k)
						MPI_Recv(&a[j][k], 1, MPI_INT, i, 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				
				cur += recvLines;

				if(cur - 1 == nn) break;
			}

	}

	//printf("%d %d %d %d \n", cur - 1, nn, rank, mm);
}

void sendComp(int nn, int mm, int** a) {

	int i, j;

	MPI_Ssend(&nn, 1, MPI_INT, top[rank], 111, MPI_COMM_WORLD);

	for(i = 1 ; i <= nn; ++i)
		for(j = 1 ; j <= mm; ++j)
			MPI_Ssend(&a[i][j], 1, MPI_INT, top[rank], 111, MPI_COMM_WORLD);
}

int maxi(int a, int b) {
	return a > b ? a : b;
}

int mini(int a, int b) { return a > b ? b : a; }

void applyFilter(int nn, int mm, int** a, Filter* f) {

	int i, j, k, l;

	int** b = NULL;

	b = (int**) calloc(nn + 2, sizeof(int*));

	for(i = 0 ; i <= nn + 1; ++i)
		*(b + i) = (int*) calloc(mm + 2, sizeof(int));

	for(i = 0; i <= nn + 1; ++i)
		for(j = 0 ; j <= mm + 1; ++j)
			b[i][j] = a[i][j];

	for(i = 1; i <= nn; ++i)
		for(j = 1; j <= mm; ++j) {
			
			a[i][j] = 0;

			for(k = -1; k <= 1; ++k)
				for(l = -1; l <= 1; ++l)
					a[i][j] += b[i + k][j + l] * f->mat[k + 1][l + 1];

			a[i][j] /= f->sum;
			a[i][j] += f->dep;

			a[i][j] = maxi(a[i][j], 0);
			a[i][j] = mini(255, a[i][j]);
		}

	for(i = 0 ; i <= nn + 1; ++i)
		free(b[i]);
	free(b);
}

void applyFilters(char* filterType, int nn, int mm, int** a) {

	
	if(strcmp(filterType, "blur") == 0)
		applyFilter(nn, mm, a, &blur);
	else if(strcmp(filterType, "sharpen") == 0)
		applyFilter(nn, mm, a, &sharpen);
	else if(strcmp(filterType, "smooth") == 0)
		applyFilter(nn, mm, a, &smooth);
	else if(strcmp(filterType, "mean_removal") == 0)
		applyFilter(nn, mm, a, &meanremove);
}

void solveTasksRoot(Task* tasks, int nrTasks, char* argv[]) {

	int i, j;

	int* stat = (int*) calloc(n, sizeof(int));

	for(i = 0 ; i < n; ++i)
		if(top[i] == 0)
			MPI_Ssend(&nrTasks, 1, MPI_INT, i, 111, MPI_COMM_WORLD);

	for(i = 0 ; i < nrTasks; ++i) {

		Myimage a;
		readImage(tasks[i].input, &a);

		sendBucket(tasks[i].effect, a.n, a.m, a.a);
		receiveComp(a.n, a.m, a.a);
		writeImage(tasks[i].output, &a);
		deallocate(a.n + 2, a.m + 2, a.a);
	}

	//termina program
	int finish = 0;
	for(i = 0 ; i < n; ++i)
		if(top[i] == rank)
			MPI_Send(&finish, 1, MPI_INT, i, 222, MPI_COMM_WORLD);

	int* auxStat = (int*) calloc(n, sizeof(int));


	for(i = 0 ; i < n; ++i)
		if(top[i] == rank) {
			for(j = 0 ; j < n; ++j)
				MPI_Recv(&auxStat[j], 1, MPI_INT, i, 222, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			mergee(auxStat, stat);
		}
	
	writeStat(argv[3], stat);
	free(stat);
	free(auxStat);

}

void solveTasks() {

	int i, j;
	int nrTasks;
	int* stat = (int*) calloc(n, sizeof(int));


	MPI_Recv(&nrTasks, 1, MPI_INT, top[rank], 111, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	for(i = 0 ; i < n ;  ++i)
		if(top[i] == rank)
			MPI_Ssend(&nrTasks, 1, MPI_INT, i, 111, MPI_COMM_WORLD);

	int nn, mm;
	int** a = NULL;
	char filterType[MAXFILENAME]; 

	for(i = 0; i < nrTasks; ++i) {

		receiveBucket(filterType, &nn, &mm, &a);
		
		sendBucket(filterType, nn - 2, mm - 2, a);

		if(nn == 0) continue;

		if(sons == 0) {
			applyFilters(filterType, nn - 2, mm - 2, a);
			stat[rank] += nn - 2;
		}

		if(sons > 0)
			receiveComp(nn - 2, mm - 2, a);

		sendComp(nn - 2, mm - 2, a);
		deallocate(nn, mm, a);

	}

	int finish = 0;
	MPI_Recv(&finish, 1, MPI_INT, top[rank], 222, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	for(i = 0 ; i < n; ++i)
		if(top[i] == rank)
			MPI_Send(&finish, 1, MPI_INT, i, 222, MPI_COMM_WORLD);

	int* auxStat = (int*) calloc(n, sizeof(int));

	for(i = 0 ; i < n; ++i)
		if(top[i] == rank) {
			for(j = 0 ; j < n; ++j)
				MPI_Recv(&auxStat[j], 1, MPI_INT, i, 222, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			mergee(auxStat, stat);
		}
	
	for(i = 0 ; i < n; ++i)
		MPI_Send(&stat[i], 1, MPI_INT, top[rank], 222, MPI_COMM_WORLD);

	free(stat);
	free(auxStat);

}

int main(int argc, char* argv[]) {


	checkArguments(argc, argv);
	initProcesses(argc, argv);

	readTopology(argv);
	waveTree();

	//printToplogy();

	if(rank == 0) {

		int nrTasks;
		Task* tasks = readImages(argv, &nrTasks);

		solveTasksRoot(tasks, nrTasks, argv);
		free(tasks);

	} else {

		solveTasks();
	}

	free(top);


	MPI_Finalize();

	return 0;
}