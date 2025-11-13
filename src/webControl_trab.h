// web_control.h
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

// Inclua seus outros headers se necessário (servo_trab.h, config.h)
#include "config.h"
#include "servo_trab.h"
#include "web.h"
#include "motorDc.h"
#include "log_trab.h"

// Declaracões externas para as variáveis de estado do carro
extern volatile int DirecaoLida;
extern volatile int velocidadeLida;

// Função para configurar o servidor web e WebSockets
void initWebServer();

// Função para limpar clientes inativos (chamar no loop())
void cleanupWebClients();

void enviaDadosClientes(String str, int valor);

void enviaDadosClientes(String str, String dado);

#endif
