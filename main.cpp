#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "functions.h"

int main() {
    srand(time(0));
    inicializaFloresta();

    // Criação das threads para os sensores
    pthread_t sensores[TAMANHO][TAMANHO];
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            int* pos = (int*)malloc(2 * sizeof(int));
            pos[0] = i;
            pos[1] = j;
            pthread_create(&sensores[i][j], NULL, sensor, pos);
        }
    }

    // Criação da thread geradora de incêndios
    pthread_t threadGerador;
    pthread_create(&threadGerador, NULL, geradorIncendio, NULL);

    // Rounds de eventos
    for (int round = 0; round < 15; round++) {
        std::cout << "\n--- Round " << round + 1 << " ---\n";
        
        // Chama a função de impressão
        imprimeFloresta();

        // Adiciona um tempo de espera entre os rounds
        sleep(5); // Espera antes do próximo round
    }

    // Finaliza as threads dos sensores
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            sensorAtivo[i][j] = false; // Marca sensores como inativos
            pthread_cond_broadcast(&condIncendio); // Acorda sensores para terminar
            pthread_join(sensores[i][j], NULL); // Aguarda a finalização
        }
    }

    pthread_cancel(threadGerador); // Finaliza a thread geradora de incêndios
    pthread_join(threadGerador, NULL); // Aguarda a finalização

    return 0;
}
