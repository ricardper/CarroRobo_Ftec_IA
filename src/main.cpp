// main.cpp
#include <Arduino.h>

#include "config.h"
#include "servo_trab.h"
#include "wifi_trab.h"
#include "webControl_trab.h"
#include "motorDc.h"
#include "serial_trab.h"
#include "andarSozinho.h"

unsigned long tempoAgora = millis();

unsigned long tempoAntes = 0;

const long timerOut = TIMEOUT_NILLIS;

void setup()
{
  Serial.begin(SERIAL_VELOCIDADE);

  initServo();

  initWifi();

  initWebServer();

  initMotoDc();
}

void loop()
{
  tempoAgora = millis();

  if (tempoAgora - tempoAntes >= timerOut)
  {
    cleanupWebClients();
    lerComando();
    moveMedicao();
    tempoAntes = tempoAgora;

    /*
    Insirir aqui para o carro andar sozinho
    ex:
    Move a direção em graus (SERVO_DIRECAO_MIN 35)->direita  (SERVO_DIRECAO_MAX 130)-> esquerda

    Exemplo toda a direção para a direita
    setServoDirecao(35);


    Mover para frente e para traz
    Valores -100 a 100
    motor desligado 0

    Exemplo 100% da velocidade para a frente
    moverMotor(100);

    Exemplo 100% da velocidade para a traz
    moverMotor(-100);

    Exemplo motor parado
    moverMotor(0);
    ou
    deslMotor();


    exemplo onde deve ser adcionada a logica para o carro andar sozinho
    OLHE OS ARQUIVOS andarSozinho.h andarSozinho.c

    desviarObstaculo();

    */


    desviarObstaculo();
    
  }

}
