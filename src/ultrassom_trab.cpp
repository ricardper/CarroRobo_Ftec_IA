
#include "config.h"

#include "ultrassom_trab.h"

// TRIG e ECHO definidos no config.h
// #define HCSR04_TRIG_PIN  26
// #define HCSR04_ECHO_PIN  27
// #define HCSR04_DIST_MAX_CM 200
// #define HCSR04_DIST_MIN_CM 5

bool initUltrassom()
{
    pinMode(HCSR04_TRIG_PIN, OUTPUT);
    pinMode(HCSR04_ECHO_PIN, INPUT);

    // Garante TRIG em nível baixo ao iniciar
    digitalWrite(HCSR04_TRIG_PIN, LOW);

    delay(50); // estabilização
    return true;
}

float lerDistanciaCmUltrasonico()
{
    // Envia pulso de trigger
    digitalWrite(HCSR04_TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(HCSR04_TRIG_PIN, HIGH);
    delayMicroseconds(10); // pulso de 10 us
    digitalWrite(HCSR04_TRIG_PIN, LOW);

    // Mede o pulso no ECHO (timeout 30 ms)
    unsigned long duracao = pulseIn(HCSR04_ECHO_PIN, HIGH, 30000UL);

    if (duracao == 0)
        return -1.0f; // sem eco / timeout

    const float FATOR_CALIBRACAO = 1.26f;

    // Fórmula clássica: distância (cm) = tempo(us) / 58
    float distancia_f = duracao / 58.0f;

    distancia_f = distancia_f * FATOR_CALIBRACAO;
    int distancia = (int)distancia_f;

    distancia = (distancia % 2 == 0) ? distancia : distancia + 1;
    // Limita dentro da faixa útil
    if (distancia < HCSR04_DIST_MIN_CM)
        distancia = HCSR04_DIST_MIN_CM;

    if (distancia > HCSR04_DIST_MAX_CM)
        distancia = HCSR04_DIST_MAX_CM;

    return distancia;
}
