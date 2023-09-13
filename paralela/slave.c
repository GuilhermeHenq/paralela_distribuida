#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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
    int finalizado = 0; 
    for (int p = 1; p < world_size; p++) {
      number_amount = 1000 + rand() % 1001;  //1000 e 2000
      for (int i = 0; i < number_amount; i++)
        numbers[i] = rand() % 100;  //0 e 99
      
      tag = rand() % 5;
      MPI_Send(&number_amount, 1, MPI_INT, p, tag, MPI_COMM_WORLD);
      MPI_Send(numbers, number_amount, MPI_INT, p, tag, MPI_COMM_WORLD);
      printf("Mestre enviou %d números com a tag %d para o Escravo %d\n", number_amount, tag, p);
      
      if (tag == 4 && !finalizado) {
        finalizado = 1;
        MPI_Send(&number_amount, 1, MPI_INT, p, tag, MPI_COMM_WORLD);
        printf("Mestre enviou a tag 4 para finalizar o Escravo %d\n", p);
        break;
      }
    }
  } else {

    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    tag = status.MPI_TAG;

    while (tag != 4) {
      MPI_Recv(&number_amount, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      tag = status.MPI_TAG;

      if (tag == 4) {
        printf("Escravo %d recebeu a tag 4 e foi finalizado!\n", world_rank);
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
        case 3: // Mediana
          if (number_amount % 2 == 1) {
            result = numbers[number_amount / 2];
          } else {
            int meio1 = numbers[(number_amount - 1) / 2];
            int meio2 = numbers[number_amount / 2];
            result = (meio1 + meio2) / 2;
          }
          break;
        default:
          // Operação desconhecida
          break;
      }

      MPI_Send(&result, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
      printf("Escravo %d devolveu %d como resultado para o Mestre com tag %d\n", world_rank, result, tag);
    }
    
  }

  MPI_Finalize();
  return 0;
}
