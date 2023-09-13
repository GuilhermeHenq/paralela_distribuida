/* Exercícios

1º Faça um programa com a estrutura mestre/escravo, onde:

 - mestre: gera uma quantidade aleatória entre 1000 e 2000 números de valores aleatórios entre 0 e 99, e envia para cada escravo.
 
 - escravo: recebe os valores gerados pelo mestre, calcula a somatória dos valores e devolve para o mestre.
 
*/
 
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {
  MPI_Status status;
  MPI_Init(NULL, NULL);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  const int MAX_NUMBERS = 2000;
  int numbers[MAX_NUMBERS];
  int number_amount;

  if (world_rank == 0) {
    int soma = 0, somaproc, somalocal = 0;
    // Pick a random amount of integers to send to process one
    srand(time(NULL));
    for(int p = 1; p < world_size; p++){
      number_amount = 1000 + (rand() / (float)RAND_MAX) * (MAX_NUMBERS - 1000);
      for(int i = 0; i < number_amount; i++)
        numbers[i] = (rand() / (float)RAND_MAX) * MAX_NUMBERS; // Para teste, substituir por 1; 
      // Send the amount of integers to process one
      MPI_Send(numbers, number_amount, MPI_INT, p, 0, MPI_COMM_WORLD);
      printf("0 sent %d numbers to %d\n", number_amount, p);
      somalocal += number_amount;
    }
    for(int p = 1; p < world_size; p++){
      MPI_Recv(&somaproc, 1, MPI_INT, p, 1, MPI_COMM_WORLD, &status);
      soma += somaproc;
    }
    printf("A soma total foi %d e a soma local foi %d\n", soma, somalocal);

  } else {
    int soma = 0, tag;
    MPI_Status status;

    MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    tag = status.MPI_TAG;
    printf("Em %d, a tag foi %d\n", world_rank, tag);
    
    // Receive at most MAX_NUMBERS from process zero
    MPI_Recv(numbers, MAX_NUMBERS, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
    // After receiving the message, check the status to determine how many
    // numbers were actually received
    MPI_Get_count(&status, MPI_INT, &number_amount);
    // Print off the amount of numbers, and also print additional information
    // in the status object
    printf("%d received %d numbers from 0. Message source = %d, tag = %d\n",
           world_rank, number_amount, status.MPI_SOURCE, status.MPI_TAG);
    for (int i = 0; i < number_amount; i++)
      soma += numbers[i];
    printf("%d somou %d\n", world_rank, soma);
    MPI_Send(&soma, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
