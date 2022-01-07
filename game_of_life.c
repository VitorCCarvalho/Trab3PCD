#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define SRAND_VALUE 1985
#define geracoes 2000
#define N 2048
#define SIZE N *N

long int get_col(long int j, long int vizinho_p){
    j += vizinho_p;
    if (j < 0)
        return j + N;
    if (j >= N)
        return j - N;
    return j;
}

long int get_row(long int i, long int vizinho_p){
    i += vizinho_p;
    if (i < 0)
        return i + N;
    if (i >= N)
        return i - N;
    return i;
}



long int get_vizinho(long int pcell, long int k, long int l){
    long int vizinho = N * get_row(pcell / N, k) + get_col(pcell % N, l);
    return vizinho;
}

long int count_vizinhos(long int *tab, int pcell){
    long int count = 0;
    for (long int k = -1; k < 2; k++){
        for (long int l = -1; l < 2; l++){
            if (k || l)
                count += tab[get_vizinho(pcell, k, l)];
        }
    }
    return count;
}

long int game_life(long int *tab){
    long int vizinhos, i, j, k;
    long int *new_tab = malloc(SIZE * sizeof(long int));
    for (i = 0; i < geracoes; i++){
        printf("geracao = %ld\n", i);
        for (j = 0; j < SIZE; j++){
            vizinhos = count_vizinhos(tab, j);

            if (vizinhos < 2 || vizinhos > 3) 
                new_tab[j] = 0;
            else if (vizinhos == 2) 
                new_tab[j] = tab[j];
            else if (vizinhos == 3) 
                new_tab[j] = 1;
        }
        for (k = 0; k < SIZE; k++){
            tab[k] = new_tab[k];
        }
    }

    long int count = 0;
    for (i = 0; i < SIZE; i++){
        if (tab[i]){
            count++;
        }
    }
    return count;
}


int main(int argc, char const *argv[]){
    long int *tab = malloc((SIZE) * sizeof(long int));
    long int res = 0;
    long int i;

    srand(SRAND_VALUE);
    for (i = 0; i < SIZE; i++){
        tab[i] = rand() % 2;
    }

    res = game_life(tab);

    printf("resultado = %li", res);
    free(tab);
    return 0;
}