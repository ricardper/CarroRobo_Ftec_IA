#ifndef _WATCHDOG_TRAB_H_
#define _WATCHDOG_TRAB_H_

#include <Arduino.h>
#include <esp_task_wdt.h>

void iniciarWatchdog();
void alimentarWatchdog();

#endif
