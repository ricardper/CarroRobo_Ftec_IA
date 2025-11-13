#ifndef _LOG_TRAB_H

#define _LOG_TRAB_H

#include <Arduino.h>
#include "config.h"
#include "webControl_trab.h"

extern bool DEBUG;

void initLog();

void logTrab(String str, int valor);

void logTrab(String str);
void ligaDesligaDebug(int valor);

#endif