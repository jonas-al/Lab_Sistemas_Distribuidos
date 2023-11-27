#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 8

void display_matrix(int matrix[][N]) {
  for (int i = 0; i < N; ++i) {
    printf("\n");
    printf("[ ");
    for (int j = 0; j < N; ++j) {
      if (j - N == -1) {
        if (matrix[i][j] > 9) printf("%i ]", matrix[i][j]);
        else printf(" %i ]", matrix[i][j]);
      }
      else {
        if (matrix[i][j] > 9) printf("%i ", matrix[i][j]);
        else printf("%i  ", matrix[i][j]);
      }
    }
  }

  printf("\n");
}

void display_matrix_sub(int matrix[][4]) {
  for (int i = 0; i < 4; ++i) {
    printf("\n");
    printf("[ ");
    for (int j = 0; j < 4; ++j) {
      if (j - 4 == -1) {
        if (matrix[i][j] > 9) printf("%i ]", matrix[i][j]);
        else printf(" %i ]", matrix[i][j]);
      }
      else {
        if (matrix[i][j] > 9) printf("%i ", matrix[i][j]);
        else printf("%i  ", matrix[i][j]);
      }
    }
  }

  printf("\n");
}

void generate_matrix(int matrix[][N]) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      matrix[i][j] = rand() % 10;
    }
  }
}

void split_matrix(
  int matrix[][N],
  int matrix_block_one[N/2][N/2],
  int matrix_block_two[N/2][N/2],
  int matrix_block_three[N/2][N/2],
  int matrix_block_four[N/2][N/2]
) {

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      if (i < N/2 && j < N/2) matrix_block_one[i][j] = matrix[i][j];
      else if (i < N/2 && j >= N/2) {
        int c = j - N/2;
        matrix_block_two[i][c] = matrix[i][j];
      }
      else if (i >= N/2 && j < N/2) {
        int l = i - N/2;
        matrix_block_three[l][j] = matrix[i][j];
      }
      else if (i >= N/2 && j >= N/2) {
        int l = i - N/2;
        int c = j - N/2;
        matrix_block_four[l][c] = matrix[i][j];
      }
    }
  }
}

void add_matrices(
  int matrizA[][N/2],
  int matrizB[][N/2],
  int matrixC[][N/2]
) {
  for (int i = 0; i < N/2; ++i) {
    for (int j = 0; j < N/2; ++j) {
      matrixC[i][j] = matrizA[i][j] + matrizB[i][j];
    }
  }
}


int main(int argc, char **argv)
{
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);

  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);


  int matrixA[N][N];
  int matrixB[N][N];
  int matrixC[N][N];

  // We are assuming at least 4 processes for this task
  if (world_size < 4)
  {
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  if (world_rank == 0)
  {
    generate_matrix(matrixA);
    generate_matrix(matrixB);


    printf("Matriz A:");
    display_matrix(matrixA);
    printf("\n\n");

    printf("Matriz B:");
    display_matrix(matrixB);
    printf("\n\n");
  }

  int subA[N/2][N/2], subB[N/2][N/2], subC[N/2][N/2];

  MPI_Scatter(matrixA, N * N / 4, MPI_INT, subA, N * N / 4, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(matrixB, N * N / 4, MPI_INT, subB, N * N / 4, MPI_INT, 0, MPI_COMM_WORLD);

  add_matrices(
      subA,
      subB,
      subC
  );

  MPI_Gather(subC, N * N / 4, MPI_INT, matrixC, N * N / 4, MPI_INT, 0, MPI_COMM_WORLD);

  if (world_rank == 0) {
    printf("Matriz Resultante C:");
    display_matrix(matrixC);
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}