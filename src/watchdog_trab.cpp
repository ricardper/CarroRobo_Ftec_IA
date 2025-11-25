#include "watchdog_trab.h"

void iniciarWatchdog()
{
    // Timeout = 3 segundos, reset automático
    esp_task_wdt_init(3, true);

    // Adiciona o loop principal ao watchdog
    esp_task_wdt_add(NULL);

    Serial.println("Watchdog de 3s iniciado.");
}

void alimentarWatchdog()
{
    esp_task_wdt_reset();
}
