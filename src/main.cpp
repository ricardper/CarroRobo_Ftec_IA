// main.cpp — VERSÃO FINAL DO PROJETO (RADAR + HCSR04 + WS)

#include <Arduino.h>
#include <esp_task_wdt.h>

#include "config.h"
#include "servo_trab.h"
#include "wifi_trab.h"
#include "webControl_trab.h"
#include "motorDc.h"
#include "serial_trab.h"
#include "andarSozinho.h"
#include "log_trab.h"
#include "sensorDistanciaLaser.h"
#include "ultrassom_trab.h"
#include "andarSozinho.h"
#include "moverSozinho.h"

// ======= Timers =======
unsigned long agora = 0;
unsigned long ultimoHouse = 0;
unsigned long ultimoRadar = 0;

const unsigned long INTERVALO_HOUSE_MS = 50;
const unsigned long INTERVALO_RADAR_MS = TIMEOUT_NILLIS;

float dist;
float distUltra;

void setup()
{
  Serial.begin(SERIAL_VELOCIDADE);
  delay(300);

  initLog();
  initServo();
  initWifi();
  initWebServer();
  initMotoDc();
  initMedicao();
  initUltrassom();
  // initAndarSozinho();
  initMoverSozinho();

  dist = 0;
  distUltra = 0;
  iniciarWatchdog();

  logTrab("Setup completo");
}

void loop()
{
  agora = millis();
  alimentarWatchdog();

  // =================== HOUSEKEEPING ===================
  if (agora - ultimoHouse >= INTERVALO_HOUSE_MS)
  {
    ultimoHouse = agora;
    cleanupWebClients();
    lerComando();
  }

  // =================== RADAR (servo + ultrassom) ===================
  if (agora - ultimoRadar >= INTERVALO_RADAR_MS)
  {
    ultimoRadar = agora;

    // 1) mover servo horizontal
    moveMedicao(); // faz o giro de 180

    // 2) obter distância

    dist = lerDistanciaCm();       // já filtrada------------------------------------------------
    int ang = getPosicaoMedicao(); // ângulo atual do servo

    // 3) pacote WS
    String payload = String(ang) + "," + String(dist, 1);

    // Serial.println(payload);

    enviaDadosClientes("radar", payload);
    // ==================== ULTRASSÔNICO ====================
    distUltra = lerDistanciaCmUltrasonico();
    // Serial.println(distUltra);
    enviaDadosClientes("ultra", (int)distUltra);
  }
  if (modoAutonomoAtivo)
  {
    moverSozinho(distUltra);
    // andarSozinhoLoop();
  }
  // =================== AUTÔNOMO (opcional) ===================
  // desviarObstaculo();
}
