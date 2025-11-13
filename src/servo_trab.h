#ifndef _SERVO_TRAB_H
#define _SERVO_TRAB_H
#include <Arduino.h>
#include "config.h"
#include "webControl_trab.h"
#include <ESP32Servo.h> //https://github.com/madhephaestus/ESP32Servo
#include "log_trab.h"

extern Servo servoDirecao;
extern Servo servoMedicao;

extern bool sentidoMedicao;
extern int posicaoMedicao;
extern bool medicaoLigado;

extern volatile int servoUltimoValor;

int initServo();

void servoSetValor(Servo *servo);

int setServoDirecao(int graus);

void servoDirecaoCentro();

int getDirecaoGraus();

void addDirecao();

void subDirecao();

void addMedicao();

void subMedicao();

void moveMedicao();

void ligaDesligaMedicao(int valor);

#endif