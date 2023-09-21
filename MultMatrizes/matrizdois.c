#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void generate_matrix(int *matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        matrix[i] = rand() % 4; // Gere valores aleatórios (ajuste conforme necessário)
    }
}

void print_matrix(int *matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d\t", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <linhas matriz1> <colunas matriz2>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }


    int matrix1_rows = atoi(argv[1]); // Número de linhas da matriz 1
    int matrix2_cols = atoi(argv[2]); // Número de colunas da matriz 2

    int condicion_rows = matrix1_rows;

    int matrix1_cols = condicion_rows; // Número de colunas da matriz 1
    int matrix2_rows = condicion_rows; // Número de linhas da matriz 2

    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != matrix1_rows) {
        if (rank == 0) {
            printf("O número de processos deve ser igual ao número de linhas da matriz 1 (%d)\n", matrix1_rows);
        }
        MPI_Finalize();
        return 1;
    }

    // Aloca espaço para as matrizes
    int *matrix1 = (int *)malloc(matrix1_rows * matrix2_cols * sizeof(int));
    int *matrix2 = (int *)malloc(matrix2_cols * matrix1_rows * sizeof(int));
    int *result = (int *)malloc(matrix1_rows * matrix2_cols * sizeof(int));

    // Gera as matrizes no processo raiz
    if (rank == 0) {
        srand(time(NULL));
        generate_matrix(matrix1, matrix1_rows, matrix1_cols);
        generate_matrix(matrix2, matrix2_rows, matrix2_cols);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Transmite a matriz 2 para todos os processos
    MPI_Bcast(matrix2, matrix2_cols * matrix1_rows, MPI_INT, 0, MPI_COMM_WORLD);


    MPI_Barrier(MPI_COMM_WORLD);
    // Divide as linhas da matriz 1 entre os processos
    int rows_per_process = matrix1_rows / size;
    int *local_matrix1 = (int *)malloc(rows_per_process * matrix2_cols * sizeof(int));


    MPI_Barrier(MPI_COMM_WORLD);
    // Envia as linhas correspondentes da matriz 1 para cada processo
    MPI_Scatter(matrix1, rows_per_process * matrix2_cols, MPI_INT, local_matrix1,
                rows_per_process * matrix2_cols, MPI_INT, 0, MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);

    // Calcula a multiplicação local
    for (int i = 0; i < rows_per_process; i++) {
        for (int j = 0; j < matrix2_cols; j++) {
            result[i * matrix2_cols + j] = 0;
            for (int k = 0; k < matrix1_cols; k++) {
                result[i * matrix2_cols + j] += local_matrix1[i * matrix1_cols + k] * matrix2[k * matrix2_cols + j];
            }
        }
    }


    MPI_Barrier(MPI_COMM_WORLD);
    // Coleta os resultados parciais no processo raiz
    MPI_Gather(result, rows_per_process * matrix2_cols, MPI_INT, result,
               rows_per_process * matrix2_cols, MPI_INT, 0, MPI_COMM_WORLD);

    // O processo raiz imprime a matriz resultante
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Matriz resultante:\n");
        print_matrix(result, matrix1_rows, matrix2_cols);


        printf("\nMatriz A:\n");
        print_matrix(matrix1, matrix1_rows, matrix1_cols);


        printf("\nMatriz B:\n");
        print_matrix(matrix2, matrix2_rows, matrix2_cols);
    }

    // Libera a memória alocada
    free(matrix1);
    free(matrix2);
    free(result);
    free(local_matrix1);

    MPI_Finalize();

    return 0;
}