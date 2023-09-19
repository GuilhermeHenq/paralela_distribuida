#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_NUMBERS 2000

//funcao para calcular mediana 
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int median_val(int *numero, int tamanho_numero) {
    sleep(3);
    qsort(numero, tamanho_numero, sizeof(int), cmpfunc);
    
    if (tamanho_numero % 2 == 0) {
        return (numero[(tamanho_numero / 2) - 1] + numero[tamanho_numero / 2]) / 2;
    } else {
        return numero[(tamanho_numero / 2)];
    }
}

//funcao para calcular soma de numero
int soma_numeros(int *numero, int tamanho_numero) {
    sleep(2);
    int soma = 0;
    for (int i = 0; i < tamanho_numero; i++) {
        soma += numero[i];
    }
    return soma;
}

//funcao para retornar o maior valor dos numeros
int valor_maximo(int *numero, int tamanho_numero) {
    sleep(1);
    int max = 0;
    for (int i = 0; i < tamanho_numero; i++) {
        if (numero[i] > max) {
            max = numero[i];
        }
    }
    return max;
}

// funcao para gerar numeros aleatorios dado um minimo e um maximo 
int gerar_aleatorio(int min, int max) {
    return min + rand() % (max - min);
}

int main(int argc, char** argv) {
  MPI_Status status;
  MPI_Init(NULL, NULL);

  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  int numbers[MAX_NUMBERS];
  int number_amount;

  //caso o world_rank seja 0, ele é o mestre!
  if (world_rank == 0) {
    srand(time(NULL));
    int tarefas = gerar_aleatorio(10, 50);

    // enviando a primeira tarefa para cada escravo
    for (int i = 1; i < world_size; i++) {    
      printf("Tarefa numero: %d \n", tarefas);

      number_amount = 1000 + rand() % 1001;  //gera um numero aleatorio entre 1000 e 2000
      for (int i = 0; i < number_amount; i++){ 
        numbers[i] = rand() % 100;  //gera um numero aleatorio entre 0 e 99
      }

      int tag_enviada = rand() % 4;

      MPI_Send(&number_amount, 1, MPI_INT, i, tag_enviada, MPI_COMM_WORLD);
      MPI_Send(numbers, number_amount, MPI_INT, i, tag_enviada, MPI_COMM_WORLD);
      printf("Mestre enviou %d números com a tag %d para o Escravo %d\n", number_amount, tag_enviada, i);

      tarefas--;
    }

    // o escravo que terminar primeiro o programa recebe e manda novamente outra tarefa para este escravo até se esgotar as tarefas
    while (tarefas > 0) {
      printf("Tarefa numero: %d \n", tarefas);
      int resultado;
      MPI_Recv(&resultado, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      printf("\t[Mestre] recebeu do escravo: %d,  o valor %d, com a tag %d \n", status.MPI_SOURCE, resultado, status.MPI_TAG);

      number_amount = 1000 + rand() % 1001;  //gera um numero aleatorio entre 1000 e 2000
      for (int i = 0; i < number_amount; i++){ 
        numbers[i] = rand() % 100;  //gera um numero aleatorio entre 0 e 99
       }

      int tag_enviada2 = rand() % 4;

      MPI_Send(&number_amount, 1, MPI_INT, status.MPI_SOURCE, tag_enviada2, MPI_COMM_WORLD);
      MPI_Send(numbers, number_amount, MPI_INT, status.MPI_SOURCE , tag_enviada2, MPI_COMM_WORLD); 
      printf("Mestre enviou %d números com a tag %d para o Escravo %d\n", number_amount, tag_enviada2, status.MPI_SOURCE);
      tarefas--;
    }

    // recebendo os ultimos resultados dos escravos e finalizando os mesmos
    for (int j = 1; j < world_size; j++) {
      int result;
      MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      printf("\t[Mestre] recebeu do escravo: %d,  o valor %d, com a tag %d \n", status.MPI_SOURCE, result, status.MPI_TAG);

      // finalizar é um valor simbolico visto que o importante é a tag 10.
      int finalizar = 50;
      int tag = 10;
      MPI_Send(&finalizar, 1, MPI_INT, j, tag, MPI_COMM_WORLD);
    }
  } 
  // nao sendo o worldrank 0, ele é um escravo
  else {
    while (1) {
      MPI_Recv(&number_amount, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      // pega a tag para comparar 
      int tag_compare = status.MPI_TAG;

      //recebe e compara se a tag recebida é 10, caso seja finaliza o mesmo.
      if (tag_compare == 10) {
        printf(" Finalizando [Escravo] %d, que recebeu a tag 10\n", world_rank);
        break;
      }

      // caso nao seja 10, ele recebe e entra em um switch para ver qual operação deve ser feita
      MPI_Recv(numbers, number_amount, MPI_INT, 0, tag_compare, MPI_COMM_WORLD, &status);
      int result = 0;

      switch (tag_compare) {
        case 0:  // Soma dos valores
          result = soma_numeros(numbers, number_amount);
          break;
        case 1:  // Média dos valores
          result = soma_numeros(numbers, number_amount) / number_amount;
          break;
        case 2:  // Maior valor
          result = valor_maximo(numbers, number_amount);
          break;
        case 3: // Mediana
          result = median_val(numbers, number_amount);
          break;
        default:// Tag nao encontrada 
          printf("A Tag %d não existe.\n", status.MPI_TAG);
          break;
      }
      
      //escravo envia o resultado obtido para o mestre
      MPI_Send(&result, 1, MPI_INT, 0, tag_compare, MPI_COMM_WORLD);
      printf("Escravo %d devolveu %d como resultado para o Mestre com tag %d\n", world_rank, result, tag_compare);
    }
    
  }

  MPI_Finalize();
  return 0;
}
