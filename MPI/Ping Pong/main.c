#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  const int PING_PONG_LIMIT = 10;

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // We are assuming 2 processes for this task
  if (world_size != 2) {
    fprintf(stderr, "World size must be two for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  // Basicamente o processo 0 envia o ping_pong_count para o processo 1 quando o ping_pong_count for par.
  // Caso contrário o processo 1 envia o ping_pong_count para o processo  0

  // O ping_pong_count defini é o contador usado para iterar no loop while
  int ping_pong_count = 0;

  // O partner_rank defini quem vai receber a mensagem
  int partner_rank = (world_rank + 1) % 2;
  while (ping_pong_count < PING_PONG_LIMIT) {
    // Se ping_pong_count for par o processo 0 envia para o processo 1
    // Se ping_pong_count for ímpar o processo 1 envia para o processo 0

    // O envio do ping_pong_count é excessncial para o funcionamento do código, pois ele
    // é responsavél por definir o processo remetente e receptor.
    printf("world_rank = %i\n", world_rank);
    if (world_rank == ping_pong_count % 2) {
      // Increment the ping pong count before you send it
      ping_pong_count++;
      MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
      printf("%d sent and incremented ping_pong_count %d to %d\n",
             world_rank, ping_pong_count, partner_rank);
    } else {
      MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD,
               MPI_STATUS_IGNORE);
      printf("%d received ping_pong_count %d from %d\n",
             world_rank, ping_pong_count, partner_rank);
    }
  }

  MPI_Finalize();
}