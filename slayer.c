#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MASTER_RANK 0
// #define NUM_VALUES_MIN 1000
// #define NUM_VALUES_MAX 2000

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size < 2) {
        fprintf(stderr, "O programa requer pelo menos 2 processos, sendo 1 mestre e 1 escravo\n");
        MPI_Finalize();
        return 1;
    }

    if (world_rank == MASTER_RANK) {
        srand(time(NULL));
        int 
        for (int slave_rank = 1; slave_rank < world_size; ++slave_rank) {
            int random = rand();
            int NUM_VALUES = (random % 1000) + 1000;
            int values[NUM_VALUES];
            for (int i = 0; i < NUM_VALUES; ++i) {
                values[i] = rand() % 100;
            }
            MPI_Send(values, NUM_VALUES, MPI_INT, slave_rank, 0, MPI_COMM_WORLD);
        }

        for (int slave_rank = 1; slave_rank < world_size; ++slave_rank) {
            int partial_sum;
            MPI_Recv(&partial_sum, 1, MPI_INT, slave_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Mestre enviou %d numeros para o escravo, e recebeu a soma de: %d do escravo: %d\n", partial_sum, slave_rank);
        }
    } else {
        int random = rand();
        int NUM_VALUES = (random % 1000) + 1000;
        int received_values[NUM_VALUES];
        MPI_Recv(received_values, NUM_VALUES, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int sum = 0;
        for (int i = 0; i < NUM_VALUES; ++i) {
            sum += received_values[i];
        }

        MPI_Send(&sum, 1, MPI_INT, MASTER_RANK, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
