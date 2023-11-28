#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Basicamente funciona com se fosse uma anel. Um processo N envia o token para o processo N+1
  // Caso N seja = 5 processo irá ocorrer assim:
  // 0 envia para o token 1
  // 1 envia para o token 2
  // 2 envia para o token 3
  // 3 envia para o token 4
  // 4 envia para o token 5
  // 5 envia para o token 0

  // O world_size é super importante, pois ele é a informação de quantos processos serão executados.

  // O token é o dado que será enviado e recebido pelos processos
  int token;

  // Quando o número do processo é diferente de 0 é recebido o token do processo world_rank - 1, ou seja, o processo anterior ao atual.
  if (world_rank != 0) {
    MPI_Recv(&token, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("Process %d received token %d from process %d\n", world_rank, token,
           world_rank - 1);
  } else {
    // Set the token's value if you are process 0
    token = -1;
  }

  // Todos os processos executam esse envio, o número do processo de destino é igual ao número do processo atual + 1
  MPI_Send(&token, 1, MPI_INT, (world_rank + 1) % world_size, 0,
           MPI_COMM_WORLD);


  // Quando o world_rank == 0 é recebido o token do processo world_size - 1, ou seja, o processo de maior número (último)
  if (world_rank == 0) {
    MPI_Recv(&token, 1, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    printf("Process %d received token %d from process %d\n", world_rank, token,
           world_size - 1);
  }
  
  MPI_Finalize();
}