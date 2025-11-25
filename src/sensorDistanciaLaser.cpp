#include "config.h"

#include "sensorDistanciaLaser.h"

// Objeto global do sensor VL53L0X
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// VARIÁVEIS DE CONFIGURAÇÃO (se não definidas em config.h)
// Essas distâncias são mantidas para manter a compatibilidade da interface
#ifndef VL53L0X_DIST_MAX_CM
#define VL53L0X_DIST_MAX_CM 200 // Maximo tipico do VL53L0X (200 cm = 2m)
#endif
#ifndef VL53L0X_DIST_MIN_CM
#define VL53L0X_DIST_MIN_CM 5
#endif
// Defina os pinos I2C se não forem os padrões (GPIO 21, 22)
// Se você está usando 18 e 19, descomente a linha abaixo e ajuste 'Wire.begin()'

bool initVL53L0X()
{
// A biblioteca Wire é necessária para a comunicação I2C
// Se estiver usando pinos customizados (ex: 18, 19), use Wire.begin(SDA_PIN, SCL_PIN)
// Caso contrário, usa os padrões do ESP32 (21, 22)
#ifdef I2C_SDA_PIN
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    // Wire.setClock(10000);
#else
    Wire.begin();
#endif

    // Tenta inicializar o sensor
    if (!lox.begin())
    {
        return false;
    }

    // Opcional: Configura o sensor para Modo Longo (Maior alcance, ~2m)
    // Se preferir velocidade, use lox.setMeasurementTimingBudget(20000); (20ms)
    // lox.startRangeLongTimeOut();

    return true;
}

bool initMedicao()
{
    return initVL53L0X();
}

float lerDistanciaCm()
{
    VL53L0X_RangingMeasurementData_t measure;

    // Realiza a medição
    lox.rangingTest(&measure, false); // 'false' desativa a impressão de debug

    // A Adafruit VL53L0X usa o status 4 para indicar falha/fora de alcance
    if (measure.RangeStatus == 4)
    {
        return 200; // Retorna -1.0f em caso de erro/timeout, compatível com a interface
    }

    // A leitura já está em milímetros (mm). Converte para centímetros (cm).
    // float distancia_cm = measure.RangeMilliMeter / 10.0f;

    // A leitura já está em milímetros (mm). Converte para centímetros (cm) SEM DECIMAL
    int distancia_cm = measure.RangeMilliMeter / 10;

    // Garante que seja número multiplo de 5

    distancia_cm = ((distancia_cm + 2) / 5) * 5;

    // Aplica os limites de faixa útil para compatibilidade de interface
    if (distancia_cm < VL53L0X_DIST_MIN_CM)
        distancia_cm = VL53L0X_DIST_MIN_CM;

    // Nota: O VL53L0X atinge no maximo ~200cm
    if (distancia_cm > VL53L0X_DIST_MAX_CM)
        distancia_cm = VL53L0X_DIST_MAX_CM;

    return distancia_cm;
}
