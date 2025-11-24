// webControl_trab.h
#ifndef _WEB_CONTROL_TRAB_H
#define _WEB_CONTROL_TRAB_H

#include <Arduino.h>
#if defined(ESP32) || defined(LIBRETINY)
#include <AsyncTCP.h>
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#elif defined(TARGET_RP2040) || defined(TARGET_RP2350) || defined(PICO_RP2040) || defined(PICO_RP2350)
#include <RPAsyncTCP.h>
#include <WiFi.h>
#endif

#include <ESPAsyncWebServer.h>

#include "config.h"
#include "servo_trab.h"
#include "web.h"
#include "motorDc.h"
#include "log_trab.h"
#include "andarSozinho.h"

// Inicializa o servidor HTTP + WebSocket
void initWebServer();

// Limpa clientes WebSocket desconectados
void cleanupWebClients();

// Envia dados para todos os clientes WebSocket
void enviaDadosClientes(String chave, int valor);
void enviaDadosClientes(String chave, String dado);

#endif
