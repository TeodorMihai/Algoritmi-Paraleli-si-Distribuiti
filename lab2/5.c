/******************************************************************************
* FILENAME: omp_matrix_strassen.c
* DESCRIPTION: Matrix multiplication using Strassen
* AUTHOR: Cristian Chilipirea
* DATE:	10-October-2016
* Matrix multiplication C = A * B
* A = B (eg for N=3)
* A = 1 1 1
      0 1 1
      0 0 1
* C = 1 2 3
      0 1 2
      0 0 1
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#define N 10

void mulMatrix(int C[][N],  int startCi, int startCj, int A[][N], int startAi, int startAj, int B[][N], int startBi, int startBj) 
{
	// Do not parallelize me
	int i, j, k;
	for(i = 0; i < N/2; i++)
		for(j = 0; j < N/2; j++)
			for(k = 0; k < N/2; k++)
				C[startCi + i][startCj + j] += A[startAi + i][startAj + k] * B[startBi + k][startBj + j];
}

void addMatrix(int C[][N], int startCi, int startCj, int A[][N], int startAi, int startAj, int B[][N], int startBi, int startBj) 
{
	// Do not parallelize me
	int i, j;
	for(i = 0; i < N/2; i++)
		for(j = 0; j < N/2; j++)
			C[startCi + i][startCj + j] = A[startAi + i][startAj + j] + B[startBi + i][startBj + j];
}

void subMatrix(int C[][N], int startCi, int startCj,  int A[][N], int startAi, int startAj, int B[][N], int startBi, int startBj) 
{
	// Do not parallelize me
	int i, j;
	for(i = 0; i < N/2; i++)
		for(j = 0; j < N/2; j++)
			C[startCi + i][startCj + j] = A[startAi + i][startAj + j] - B[startBi + i][startBj + j];
}

int main (int argc, char ** argv)  {
	int A[N][N];
	int B[N][N];
	int C[N][N];	
	int M1[N][N];
	int AUXM11[N][N];
	int AUXM12[N][N];
	int M2[N][N];
	int AUXM21[N][N];
	int M3[N][N];
	int AUXM31[N][N];
	int M4[N][N];
	int AUXM41[N][N];
	int M5[N][N];
	int AUXM51[N][N];
	int M6[N][N];
	int AUXM61[N][N];
	int AUXM62[N][N];
	int M7[N][N];
	int AUXM71[N][N];
	int AUXM72[N][N];
	int i, j, k;

	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			C[i][j] = 0;
			M1[i][j] = 0;
			AUXM11[i][j] = 0;
			AUXM12[i][j] = 0;
			M2[i][j] = 0;
			AUXM21[i][j] = 0;
			M3[i][j] = 0;
			AUXM31[i][j] = 0;
			M4[i][j] = 0;
			AUXM41[i][j] = 0;
			M5[i][j] = 0;
			AUXM51[i][j] = 0;
			M6[i][j] = 0;
			AUXM61[i][j] = 0;
			AUXM62[i][j] = 0;
			M7[i][j] = 0;
			AUXM71[i][j] = 0;
			AUXM72[i][j] = 0;
			if(i <= j) {
				A[i][j] = 1;
				B[i][j] = 1;
			}
			else {
				A[i][j] = 0;
				B[i][j] = 0;
			}
		}
	}

	/* 
		Parallelize using sections.
		You might need multiple #pragma omp sections with multiple #pragma omp section inside them.
		add(/sub/mul)Matrix all have the first parameter the output and the other two the input.
		Make sure you do not make call to a function that reads with one to a function that writes 
			to the same location in RAM on different threads 
			(different section blocks of the same sections region)
	*/

	#pragma omp parallel sections
	{

		#pragma omp section 
		{
			addMatrix(AUXM11, 0, 0, A, 0, 0, A, N/2, N/2);//A11 + A22 
			addMatrix(AUXM12, 0, 0, B, 0, 0, B, N/2, N/2);//B11 + B22
			mulMatrix(M1, 0, 0, AUXM11, 0, 0, AUXM12, 0, 0);//(A11 + A22) * (B11 * B22)
		}

		#pragma omp section
		{
			addMatrix(AUXM21, 0,0, A, N/2, 0, A, N/2, N/2);
			mulMatrix(M2, 0, 0, AUXM21, 0, 0, B, 0, 0);//M2
		}

		#pragma omp section
		{

			subMatrix(AUXM31, 0, 0, B, 0, N/2, B, N/2, N/2);
			mulMatrix(M3, 0, 0, A, 0, 0, AUXM31, 0, 0);
		}

		#pragma omp section
		{

			subMatrix(AUXM41, 0, 0, B, N/2, 0, B, 0, 0);
			mulMatrix(M4, 0, 0, A, N/2, N/2, AUXM41, 0, 0);
		}

		#pragma omp section
		{
			addMatrix(AUXM51, 0,0, A, 0, 0, A, 0, N/2);
			mulMatrix(M5, 0, 0, AUXM51, 0, 0, B, N/2, N/2);
		}	

		#pragma omp section
		{
			subMatrix(AUXM61, 0, 0, A, N/2, 0, A, 0, 0);
			addMatrix(AUXM62, 0, 0, B, 0, 0, B, 0, N/2);
			mulMatrix(M6, 0, 0, AUXM61, 0, 0, AUXM62, 0, 0);
		}

		#pragma omp section
		{
			subMatrix(AUXM71, 0, 0, A, 0, N/2, A, N/2, N/2);
			addMatrix(AUXM72, 0, 0, B, N/2, 0, B, N/2, N/2);
			mulMatrix(M7, 0, 0, AUXM71, 0, 0, AUXM72, 0, 0);
		}
	}


	#pragma omp parallel sections
	{

		#pragma omp section
		{
	 		addMatrix(C, 0, 0, M1, 0, 0, M4, 0, 0);
			subMatrix(C, 0, 0, C, 0, 0, M5, 0, 0);
			addMatrix(C, 0, 0, C, 0, 0, M7, 0, 0);
		}

		#pragma omp section
		{

			addMatrix(C, 0, N/2, M3, 0, 0, M5, 0, 0);
			addMatrix(C, N/2, 0, M2, 0, 0, M4, 0, 0);
		}

		#pragma omp section
		{

			subMatrix(C, N/2, N/2, M1, 0, 0, M2, 0, 0);
			addMatrix(C, N/2, N/2, C, N/2, N/2, M3, 0, 0);
			addMatrix(C, N/2, N/2, C, N/2, N/2, M6, 0, 0);
		}
	}

	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			printf("%3i ",C[i][j]);
		}
		printf("\n");
	}
	return 0;
}