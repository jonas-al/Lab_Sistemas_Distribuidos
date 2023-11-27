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


void join_matrices(
  int matrix[][N],
  int matrix_block_one[N/2][N/2],
  int matrix_block_two[N/2][N/2],
  int matrix_block_three[N/2][N/2],
  int matrix_block_four[N/2][N/2]
) {

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      if (i < N/2 && j < N/2) matrix[i][j] = matrix_block_one[i][j];
      else if (i < N/2 && j >= N/2) {
        int c = j - N/2;
        matrix[i][j] = matrix_block_two[i][c];
      }
      else if (i >= N/2 && j < N/2) {
        int l = i - N/2;
        matrix[i][j] = matrix_block_three[l][j];
      }
      else if (i >= N/2 && j >= N/2) {
        int l = i - N/2;
        int c = j - N/2;
        matrix[i][j] = matrix_block_four[l][c];
      }
    }
  }
}

void add_matrices(int matrizA[][N/2], int matrizB[][N/2], int matrixC[][N/2]) {
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

  // Instanciando as matrizes A, B e C
  int matrixA[N][N];
  int matrixB[N][N];
  int matrixC[N][N];
  // Gerando as matrizes A e B
  generate_matrix(matrixA);
  generate_matrix(matrixB);

  // We are assuming at least 4 processes for this task
  if (world_size < 4)
  {
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  if (world_rank == 0)
  {
    printf("Matriz A:");
    display_matrix(matrixA);
    printf("\n\n");

    printf("Matriz B:");
    display_matrix(matrixB);
    printf("\n\n");
  }

  int subA[N/2][N/2], subB[N/2][N/2], subC1_1[N/2][N/2], subC1_2[N/2][N/2], subC2_1[N/2][N/2], subC2_2[N/2][N/2];

  
  // Rank 1
  if (world_rank == 1) {
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        if (i < N/2 && j >= N/2) {
          subA[i][j - N/2] = matrixA[i][j];
          subB[i][j - N/2] = matrixB[i][j];
        }
      }
    }

    // Somando as sub matrizes A1_2 e B1_2
    add_matrices(subA, subB, subC1_2);
    for (int i = 0; i < N/2; ++i) {
      for (int j = 0; j < N/2; ++j) {
        MPI_Send(&subC1_2[i][j], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
    }
  }

  // Rank 2
  else if (world_rank == 2) {
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        if (i >= N/2 && j < N/2) {
          subA[i - N/2][j] = matrixA[i][j];
          subB[i - N/2][j] = matrixB[i][j];
        }
      }
    }

    // Somando as sub matrizes A2_1 e B2_1
    add_matrices(subA, subB, subC2_1);
    for (int i = 0; i < N/2; ++i) {
      for (int j = 0; j < N/2; ++j) {
        MPI_Send(&subC2_1[i][j], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
    }
  }

  // Rank 3
  else if (world_rank == 3) {
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        if (i >= N/2 && j >= N/2) {
          subA[i - N/2][j - N/2] = matrixA[i][j];
          subB[i - N/2][j - N/2] = matrixB[i][j];
        }
      }
    }

    // Somando as sub matrizes A2_2 e B2_2
    add_matrices(subA, subB, subC2_2);
    for (int i = 0; i < N/2; ++i) {
      for (int j = 0; j < N/2; ++j) {
        MPI_Send(&subC2_2[i][j], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      }
    }
  }

  // Rank 0
  else if (world_rank == 0) {
    // Separando as sub matrizes A1_1 e B1_1
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        if (i < N/2 && j < N/2) {
          subA[i][j] = matrixA[i][j];
          subB[i][j] = matrixB[i][j];
        }
      }
    }

    // Somando as sub matrizes A1_1 e B1_1
    add_matrices(subA, subB, subC1_1);


    
    
    // Recebendo os valores calculados por cada processo
    for (int i = 0; i < N/2; ++i) {
      for (int j = 0; j < N/2; ++j) {
        MPI_Recv(&subC1_2[i][j], 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }

    for (int i = 0; i < N/2; ++i) {
      for (int j = 0; j < N/2; ++j) {
        MPI_Recv(&subC2_1[i][j], 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }

    for (int i = 0; i < N/2; ++i) {
      for (int j = 0; j < N/2; ++j) {
        MPI_Recv(&subC2_2[i][j], 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }

    // Juntando os grupos da matriz C
    join_matrices(
      matrixC,
      subC1_1, subC1_2,
      subC2_1, subC2_2
    );

    //Exibindo a matriz resultante C
    printf("Matriz Resultante C:");
    display_matrix(matrixC);
  }


  MPI_Finalize();
  return EXIT_SUCCESS;
}