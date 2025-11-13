// serial_trab.h
#ifndef _SERIAL_TRAB_H
#define _SERIAL_TRAB_H
#include <Arduino.h>
#include "servo_trab.h"
#include "motorDc.h"
#include "log_trab.h"

void lerComando();

void processarComando(String entradaSerial);

#endif