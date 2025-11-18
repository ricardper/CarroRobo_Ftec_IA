
#ifndef ANDAR_SOZINHO_H
#define ANDAR_SOZINHO_H

#include <Arduino.h>
#include "servo_trab.h"
#include "motorDc.h"
#include "ultrassom_trab.h"
#include "config.h"

extern bool modoAutonomoAtivo;
// Inicializa buffers, índices e estado
void initAndarSozinho();

// Loop principal chamado na main()
void andarSozinhoLoop();
void ligaDesligaAutonomo(int v);

#endif
