#ifndef VL53L0X_TRAB_H
#define VL53L0X_TRAB_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

// Inicializa o sensor VL53L0X (I2C)
// Retorna true em sucesso, false em falha.
bool initVL53L0X();

bool initMedicao();

// Retorna distância em cm.
// Retorna -1.0f em caso de erro na leitura ou timeout.
float lerDistanciaCm();

#endif