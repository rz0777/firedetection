#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "functions.h"

int main() {
    srand(time(0));
    inicializaFloresta();

    pthread_t sensores[TAMANHO][TAMANHO];
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            int* pos = (int*)malloc(2 * sizeof(int));
            pos[0] = i;
            pos[1] = j;
            pthread_create(&sensores[i][j], NULL, sensor, pos);
        }
    }

    pthread_t threadGerador;
    pthread_create(&threadGerador, NULL, geradorIncendio, NULL);

    for (int round = 0; round < 15; round++) {
        std::cout << "\n--- Round " << round + 1 << " ---\n";
        imprimeFloresta();
        sleep(5);
    }

    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            sensorAtivo[i][j] = false;
            pthread_cond_broadcast(&condIncendio);
            pthread_join(sensores[i][j], NULL);
        }
    }

    pthread_cancel(threadGerador);
    pthread_join(threadGerador, NULL);

    return 0;
}
