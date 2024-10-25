#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include "functions.h"

Nodo floresta[TAMANHO][TAMANHO];
bool sensorAtivo[TAMANHO][TAMANHO];
pthread_mutex_t mutexFloresta = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condIncendio = PTHREAD_COND_INITIALIZER;

void inicializaFloresta() {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            floresta[i][j].tipo = 'T';  // todos os nodos são sensores inicialmente
            sensorAtivo[i][j] = true;
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

        // verifica incêndios locais e transmite informações para sensores vizinhos
        if (floresta[x][y].tipo == '@') {
            Incendio inc = {x, y};
            floresta[x][y].informacoesIncendio.push_back(inc);

            // transmite a informação aos vizinhos
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = x + dx, ny = y + dy;

                    // garante que o sensor não envie para quem transmitiu anteriormente
                    if ((dx != 0 || dy != 0) && nx >= 0 && nx < TAMANHO && ny >= 0 && ny < TAMANHO &&
                        floresta[nx][ny].tipo == 'T') {

                        floresta[nx][ny].informacoesIncendio.push_back(inc);
                    }
                }
            }

            if (ehBorda(x, y)) {
                floresta[x][y].tipo = '/';
                floresta[x][y].informacoesIncendio.clear();
                std::cout << "Central apagou o incêndio [" << x << ", " << y << "]\n";
            }

            floresta[x][y].informacoesIncendio.clear();
        }

        pthread_mutex_unlock(&mutexFloresta);
        sleep(1);
    }
    return NULL;
}

void* geradorIncendio(void* arg) {
    while (true) {
        sleep(3);
        int x = rand() % TAMANHO;
        int y = rand() % TAMANHO;

        pthread_mutex_lock(&mutexFloresta);

        if (floresta[x][y].tipo == 'T') {
            floresta[x][y].tipo = '@';
            std::cout << "Incêndio iniciado em [" << x << ", " << y << "]\n";
            pthread_cond_broadcast(&condIncendio);
        }

        pthread_mutex_unlock(&mutexFloresta);
        imprimeFloresta();
    }
}

void imprimeFloresta() {
    pthread_mutex_lock(&mutexFloresta);
    std::cout << "Estado da floresta:\n";
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            std::cout << floresta[i][j].tipo << " ";
        }
        std::cout << "\n";
    }
    pthread_mutex_unlock(&mutexFloresta);
}

bool ehBorda(int x, int y) {
    return (x == 0 || x == TAMANHO - 1 || y == 0 || y == TAMANHO - 1);
}
