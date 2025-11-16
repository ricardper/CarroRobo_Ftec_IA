#ifndef ULTRASSOM_TRAB_H
#define ULTRASSOM_TRAB_H

#include <Arduino.h>
#include "config.h"

// Inicializa pinos e configura HC-SR04
void initUltrassom();

// Retorna distância em cm (0 a HCSR04_DIST_MAX_CM)
// Retorna -1 se não houver eco (timeout)
float lerDistanciaCm();

#endif