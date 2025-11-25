#ifndef _MOVER_SOZINHO_H_
#include "config.h"
#include "servo_trab.h"
#include "ultrassom_trab.h"

#define _DISTANCIA_MINIMA_ 50
#define _DISTANCIA_MINIMA_RECUO_ 30
#define _ALGULO_FRENTE_MIN_ 60

#define _ALGULO_FRENTE_MAX_ 120

#define _TEMPO_RECUO_ 500
#define _TEMPO_CURVA_ 500

#define _VELOCIDADE_RECUO_ 70

#define _VELOCIDADE_CURVA_ 70

#define _VELOCIDADE_FRENTE_ 50

#define _INTERVALO_TEMPO_FRENTE 800

extern int recuo;
extern int direcaoAnterior;

extern unsigned long tempoAgora;
extern unsigned long ultimoTempo;
// extern unsigned long ultimoRadar = 0;

void initMoverSozinho();

void moverSozinho(float distUlt);

int medirFrente();

void macroRecuoCurvaDireita(int tempoMilisegundos);

void macroRecuoCurvaEsquerda(int tempoMilisegundos);

void macroCurvaDireita(int tempoMilisegundos);

void macroCurvaEsquerda(int tempoMilisegundos);

void curva(float distancia);
void curvarRecuo(float distancia);

#endif
