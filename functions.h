#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <pthread.h>

const int TAMANHO = 30; // Tamanho da matriz
extern char floresta[TAMANHO][TAMANHO];
extern bool sensorAtivo[TAMANHO][TAMANHO];
extern pthread_mutex_t mutexFloresta;
extern pthread_cond_t condIncendio;

void inicializaFloresta();
void* sensor(void* arg);
void* geradorIncendio(void* arg);
void imprimeFloresta();
bool ehBorda(int x, int y);

#endif
