#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>
#include <vector>

const int TAMANHO = 30; // Tamanho da matriz

// Estrutura que representa cada célula da floresta
struct Nodo {
    char tipo; // Tipo do nodo ('T' para sensor, '@' para incêndio, '/' para apagado)
    std::vector<std::pair<int, int>> infoIncendios; // Informações sobre posições de incêndios
};

extern Nodo floresta[TAMANHO][TAMANHO];
extern bool sensorAtivo[TAMANHO][TAMANHO];
extern pthread_mutex_t mutexFloresta;
extern pthread_cond_t condIncendio;

void inicializaFloresta();
void* sensor(void* arg);
void* geradorIncendio(void* arg);
void imprimeFloresta();
bool ehBorda(int x, int y);

#endif
