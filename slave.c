#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NUMBERS 2000

int main(int argc, char** argv) {
  MPI_Status status;
  MPI_Init(NULL, NULL);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  int numbers[MAX_NUMBERS];
  int number_amount;
  int tag;

  if (world_rank == 0) {
    srand(time(NULL));
    for (int p = 1; p < world_size; p++) {
      number_amount = 1000 + rand() % 1001;  // Gera números entre 1000 e 2000
      for (int i = 0; i < number_amount; i++)
        numbers[i] = rand() % 100;  // Gera números aleatórios entre 0 e 99
      
      // Escolhe uma operação aleatória (0 a 2) e envia com a tag correspondente
      tag = rand() % 3;
      MPI_Send(&number_amount, 1, MPI_INT, p, tag, MPI_COMM_WORLD);
      MPI_Send(numbers, number_amount, MPI_INT, p, tag, MPI_COMM_WORLD);
      printf("Mestre enviou %d números com a tag %d para o Escravo %d\n", number_amount, tag, p);
    }

    // Envia mensagem de finalização para todos os escravos
    for (int p = 1; p < world_size; p++) {
      tag = 10;
      MPI_Send(&number_amount, 1, MPI_INT, p, tag, MPI_COMM_WORLD);
    }
  } else {
    while (1) {
      MPI_Recv(&number_amount, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      tag = status.MPI_TAG;

      if (tag == 10) {
        // Escravo recebeu mensagem de finalização e sai do loop
        break;
      }

      MPI_Recv(numbers, number_amount, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

      int result = 0;
      switch (tag) {
        case 0:  // Soma dos valores
          for (int i = 0; i < number_amount; i++)
            result += numbers[i];
          break;
        case 1:  // Média dos valores
          for (int i = 0; i < number_amount; i++)
            result += numbers[i];
          result /= number_amount;
          break;
        case 2:  // Maior valor
          for (int i = 0; i < number_amount; i++)
            if (numbers[i] > result)
              result = numbers[i];
          break;
        default:
          // Operação desconhecida
          break;
      }

      // Envia o resultado de volta para o mestre
      MPI_Send(&result, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    }
  }

  MPI_Finalize();
  return 0;
}
