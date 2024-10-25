#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <cmath>
#include "functions.h"

Nodo floresta[TAMANHO][TAMANHO];
bool sensorAtivo[TAMANHO][TAMANHO];
pthread_mutex_t mutexFloresta = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condIncendio = PTHREAD_COND_INITIALIZER;

void inicializaFloresta() {
    for (int i = 0; i < TAMANHO; i++) {
        for (int j = 0; j < TAMANHO; j++) {
            floresta[i][j].tipo = 'T';
            sensorAtivo[i][j] = true;
        }
    }
}

double calculaDistancia(int x1, int y1, int x2, int y2) {
    return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}


void* sensor(void* arg) {
    int* pos = (int*)arg;
    int x = pos[0];
    int y = pos[1];
    free(pos);

    while (sensorAtivo[x][y]) {
        pthread_mutex_lock(&mutexFloresta);

        // VERIFICA E PROCESSAS INFORMAÇÕES DE INCÊNDIOS DO SENSOR ATUAL
        for (auto it = floresta[x][y].infoIncendios.begin(); it != floresta[x][y].infoIncendios.end();) {
            int incX = it->first;
            int incY = it->second;

            
            if (floresta[incX][incY].tipo != '@') {
                it = floresta[x][y].infoIncendios.erase(it);
                continue;
            }

            double distanciaAtual = calculaDistancia(x, y, incX, incY);

            // SE ESTÁ NA BORDA APAGA INCÊNDIO
            if (ehBorda(x, y)) {
                floresta[incX][incY].tipo = '/';
                it = floresta[x][y].infoIncendios.erase(it);
                std::cout << "Central apagou o incêndio em [" << incX << ", " << incY << "]\n";
            } else {
                //TRANSMITE A INFORMAÇÃO PARA OS SENSORES QUE SE DISTANCIAM DO INCÊNDIO
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        int nx = x + dx;
                        int ny = y + dy;
                        if ((dx != 0 || dy != 0) && nx >= 0 && nx < TAMANHO && ny >= 0 && ny < TAMANHO &&
                            floresta[nx][ny].tipo == 'T') {
                            
                            
                            double distanciaVizinho = calculaDistancia(nx, ny, incX, incY);

                            if (distanciaVizinho > distanciaAtual) {
                                floresta[nx][ny].infoIncendios.push_back({incX, incY});
                            }
                        }
                    }
                }
                ++it; 
            }
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

            //INFORMAÇÃO DO INCÊNDIO AOS NODOS AO REDOR
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < TAMANHO && ny >= 0 && ny < TAMANHO && floresta[nx][ny].tipo == 'T') {
                        floresta[nx][ny].infoIncendios.push_back({x, y});
                    }
                }
            }
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
        std::cout << std::endl;
    }
    pthread_mutex_unlock(&mutexFloresta);
}

bool ehBorda(int x, int y) {
    return (x == 0 || x == TAMANHO - 1 || y == 0 || y == TAMANHO - 1);
}
