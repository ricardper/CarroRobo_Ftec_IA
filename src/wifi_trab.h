#ifndef _WIFI_TRAB_H
#define _WIFI_TRAB_H
#include <Arduino.h>
#include "config.h"
#include <WiFi.h>
#include "log_trab.h" // Incluído, assumindo que esta é sua biblioteca de log

// Inicializa a rede no modo Cliente ou AP, baseado na macro WIFI_MODE_AP
// Retorna 0 (int) em sucesso, ou um código de erro (se necessário)
int initWifi();

// Imprime o status da conexão (SSID, IP, etc.)
void printWifiConect();

#endif