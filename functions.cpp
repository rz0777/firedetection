#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include "functions.h"

// Definições das variáveis globais
char floresta[TAMANHO][TAMANHO];
bool sensorAtivo[TAMANHO][TAMANHO];
pthread_mutex_t mutexFloresta = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condIncendio = PTHREAD_COND_INITIALIZER;

void inicializaFloresta() {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            floresta[i][j] = 'T';  // Todas as células são sensores
            sensorAtivo[i][j] = true;  // Todos os sensores estão ativos
        }
    }
}

void* sensor(void* arg) {
    int* pos = (int*)arg;
    int x = pos[0];
    int y = pos[1];
    free(pos);

    while (sensorAtivo[x][y]) {
        pthread_mutex_lock(&mutexFloresta);
        
        // Aguardando notificação de incêndio
        while (floresta[x][y] != '@' && sensorAtivo[x][y]) {
            pthread_cond_wait(&condIncendio, &mutexFloresta);
        }

        // Verifica se há fogo na própria célula
        if (floresta[x][y] == '@') {
            std::cout << "Sensor em [" << x << ", " << y << "] detectou incêndio!" << std::endl;

            // Propaga a informação do incêndio para os vizinhos
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if ((dx != 0 || dy != 0) && (abs(dx) + abs(dy) == 1) && // Apenas vizinhos
                        x + dx >= 0 && x + dx < TAMANHO && 
                        y + dy >= 0 && y + dy < TAMANHO) { // Limita aos índices válidos

                        // Notifica os vizinhos sobre a localização do incêndio
                        std::cout << "Informação do incêndio propagada para [" << x + dx << ", " << y + dy << "]!" << std::endl;

                        // Aqui, ao invés de marcar como '@', retorna ao estado de sensor 'T'
                        floresta[x + dx][y + dy] = 'T'; 
                    }
                }
            }

            // Se o sensor está na borda, apaga o incêndio
            if (ehBorda(x, y)) {
                floresta[x][y] = 'T';  // Apaga o incêndio retornando a célula ao estado de sensor
                std::cout << "Incêndio em [" << x << ", " << y << "] apagado pela borda!" << std::endl;
            }
        }

        pthread_mutex_unlock(&mutexFloresta);
        sleep(1);  // Espera antes da próxima checagem
    }

    return NULL;
}

void* geradorIncendio(void* arg) {
    while (true) {
        sleep(3);  // Intervalo de 3 segundos para gerar incêndios

        pthread_mutex_lock(&mutexFloresta);
        
        // Conta quantos incêndios estão ativos
        int numIncendiosAtivos = 0;
        for (int i = 0; i < TAMANHO; i++) {
            for (int j = 0; j < TAMANHO; j++) {
                if (floresta[i][j] == '@') {
                    numIncendiosAtivos++;
                }
            }
        }

        // Gera um novo incêndio se houver menos de 4 ativos
        if (numIncendiosAtivos < 4) {
            int x = rand() % TAMANHO;
            int y = rand() % TAMANHO;

            if (floresta[x][y] == 'T') {
                floresta[x][y] = '@';  // Marca a célula como incêndio
                std::cout << "Incêndio iniciado em [" << x << ", " << y << "]!" << std::endl;

                // Notifica todos os sensores que devem verificar
                pthread_cond_broadcast(&condIncendio);
            }
        }

        pthread_mutex_unlock(&mutexFloresta);
        imprimeFloresta();  // Imprime a floresta após gerar o incêndio
    }
}

void imprimeFloresta() {
    pthread_mutex_lock(&mutexFloresta);
    std::cout << "Estado da floresta:\n";
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            std::cout << floresta[i][j] << " ";
        }
        std::cout << std::endl;
    }
    pthread_mutex_unlock(&mutexFloresta);
}

bool ehBorda(int x, int y) {
    return (x == 0 || x == TAMANHO - 1 || y == 0 || y == TAMANHO - 1);
}

void* transmitirInfo(void* arg){
    
}
