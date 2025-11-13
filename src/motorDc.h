#ifndef _MOTOR_DC_H

#define _MOTOR_DC_H
#include <Arduino.h>
#include "config.h"
#include <L293D.h>
#include "webControl_trab.h"
#include "log_trab.h"

#ifndef PWM_MOTOR_FREQUENCY
#define PWM_MOTOR_FREQUENCY 200
#endif

#ifndef PWM_MOTOR_RESOLUTION
#define PWM_MOTOR_RESOLUTION 8
#endif

extern L293D motor1;

extern volatile int velocidadeAtual;

int initMotoDc();

void moverFrente(int valor);

void moverTraz(int valor);

void moverMotor(int valor);

void subVelocidade();
void addVelocidade();
void deslMotor();

#endif