
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>


#define geracoes 2000
#define N_DIM 2048
#define N N_DIM * N_DIM
#define SRAND_VALUE 1985

int soma_tab(int *tab) {
    int i, count;
    for (i = 0; i < N; i++) {
        count += tab[i];
    }
    return count;
}


int count_vizinhos(int *tab, int x) {
    int count = 0;

    int first_row;
    int last_row;
    int first_col;
    int last_col;

    if(x < N_DIM){
        first_row = 1;
    } else first_row = 0;

    if(x >= N - N_DIM){
        last_row = 1;
    } else last_row = 0;

    if(x % N_DIM == 0){
        first_col = 1;
    } else first_col = 0;

    if( (x + 1) % N_DIM == 0){
        last_col = 1;
    } else last_col = 0;

    if (first_row) {
        if (first_col) {
            count = tab[N-1] + tab[N-N_DIM] + tab[N-N_DIM+1] + tab[1] + tab[N_DIM+1] + tab[N_DIM] + tab[N_DIM+N_DIM-1] + tab[N_DIM-1];
        } 
        
        else if (last_col) {
            count = tab[N-2] + tab[N-1] + tab[N-N_DIM] + tab[0] + tab[x+1] + tab[x+N_DIM] + tab[x+N_DIM-1] + tab[x-1];
        } 
        
        else {
            count = tab[x+N-N_DIM-1] + tab[x+N-N_DIM] + tab[x+N-N_DIM+1] + tab[x+1] + tab[x+N_DIM+1] + tab[x+N_DIM] + tab[x+N_DIM-1] + tab[x-1];
        }

    }

    else if (last_row) {
        if (first_col) {
            count = tab[x-1] + tab[x-N_DIM] + tab[x-N_DIM+1] + tab[x+1] + tab[1] + tab[0] + tab[N_DIM-1] + tab[N-1];
        }
        
        else if (last_col) {
            count = tab[x-1-N_DIM] + tab[x-N_DIM] + tab[x-N_DIM-N_DIM+1] + tab[x-N_DIM+1] + tab[0] + tab[N_DIM-1] + tab[N_DIM-2] + tab[x-1];
        }  
        
        else {
            count = tab[x-N_DIM-1] + tab[x-N_DIM] + tab[x-N_DIM+1] + tab[x+1] + tab[x+N_DIM-N+1] + tab[x+N_DIM-N] + tab[x+N_DIM-N-1] + tab[x-1];
        }

    }
    
    else {
        if (first_col) {
            count = tab[x-1] + tab[x-N_DIM] + tab[x-N_DIM+1] + tab[x+1] + tab[x+N_DIM+1] + tab[x+N_DIM] + tab[x+N_DIM+N_DIM-1] + tab[x+N_DIM-1];
        } 
        
        else if (last_col) {
            count = tab[x-N_DIM-1] + tab[x-N_DIM] + tab[x+1-N_DIM-N_DIM] + tab[x+1-N_DIM] + tab[x+1] + tab[x+N_DIM] + tab[x+N_DIM-1] + tab[x-1];
        } 
        
        else {
            count = tab[x-N_DIM-1] + tab[x-N_DIM] + tab[x-N_DIM+1] + tab[x+1] + tab[x+N_DIM+1] + tab[x+N_DIM] + tab[x+N_DIM-1] + tab[x-1];
        }

    }

    return count;
}

int main(int argc, char** argv){
    int i, j;
    int cluster_size, process_rank;
    
    float time_exec = 0.0;
    float aux;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &cluster_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    MPI_Barrier(MPI_COMM_WORLD);

    int *tab = (int*)malloc(N * sizeof(int));

    int chunk = N / cluster_size;
    int *chunk_tab = (int*)malloc(chunk * sizeof(int));

    
    for (i = 0; i < chunk; i++) {
        chunk_tab[i] = rand() % 2;
    }

    MPI_Gather(chunk_tab, chunk, MPI_INT, tab, chunk, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    if (process_rank == 0) {
        printf("Condicao inicial: %d\n", soma_tab(tab));
    }

    MPI_Bcast(tab, N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    aux = MPI_Wtime();
    time_exec -= aux;

    int step = process_rank * chunk;

    for (i = 0; i < geracoes; i++) {
        for (j = 0; j < chunk; j++) {
            int tab_ref = j + step;
            int num_vizinhos = count_vizinhos(tab, tab_ref);

            if (tab[tab_ref] == 1) { 
                if (num_vizinhos < 2 || num_vizinhos > 3) {
                    chunk_tab[j] = 0;
                } else {
                    chunk_tab[j] = 1;
                }
            } else { 
                if (num_vizinhos == 3) {
                    chunk_tab[j] = 1;
                } else {
                    chunk_tab[j] = 0;
                }
            }
        }

        MPI_Gather(chunk_tab, chunk, MPI_INT, tab, chunk, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        if (process_rank == 0) {
            printf("Geracao %d: %d\n", i+1, soma_tab(tab));
        }

        MPI_Bcast(tab, N, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    time_exec += MPI_Wtime();

    if (process_rank == 0) {
        printf("Tempo de execução: %.2f segundos\n", time_exec);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    free(tab);
    free(chunk_tab);

    MPI_Finalize();
    return 0;
}