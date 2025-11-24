// main.cpp — VERSÃO FINAL DO PROJETO (RADAR + HCSR04 + WS)

#include <Arduino.h>

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

// ======= Timers =======
unsigned long agora = 0;
unsigned long ultimoHouse = 0;
unsigned long ultimoRadar = 0;

const unsigned long INTERVALO_HOUSE_MS = 100;
const unsigned long INTERVALO_RADAR_MS = TIMEOUT_NILLIS;

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
  initAndarSozinho();

  logTrab("Setup completo");
}

void loop()
{
  agora = millis();

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

    float dist = lerDistanciaCm(); // já filtrada------------------------------------------------
    int ang = getPosicaoMedicao(); // ângulo atual do servo

    // 3) pacote WS
    String payload = String(ang) + "," + String(dist, 1);

    Serial.println(payload);

    enviaDadosClientes("radar", payload);
    // ==================== ULTRASSÔNICO ====================
    float distUltra = lerDistanciaCmUltrasonico();
    Serial.println(distUltra);
    enviaDadosClientes("ultra", (int)distUltra);
  }

  // =================== AUTÔNOMO (opcional) ===================
  // desviarObstaculo();

  if (modoAutonomoAtivo)
  {
    andarSozinhoLoop();
  }
}
