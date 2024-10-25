#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>
#include <vector>

const int TAMANHO = 30;

struct Incendio {
    int x, y;
};
//tipo do nodo: 'T' para sensor, '@' para incêndio, '/' para apagado e lista de incêndios que este nodo detectou
struct Nodo {
    char tipo; 
    std::vector<Incendio> informacoesIncendio;
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
