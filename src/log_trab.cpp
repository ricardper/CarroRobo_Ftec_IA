#include "log_trab.h"

bool DEBUG = true;

void initLog()
{
}

void ligaDesligaDebug(int valor)
{

    if (valor == 0)
    {
        logTrab("DEBUG DESLIGADO");
        DEBUG = false;
    }
    else
    {
        DEBUG = true;
        logTrab("DEBUG LIGADO");
    }
}

void logTrab(String str, int valor)
{
    if (DEBUG)
    {
        Serial.println(str + " " + valor);
        enviaDadosClientes("console", "->" + str + "_" + valor);
    }
}

void logTrab(String str)
{
    if (DEBUG)
    {
        Serial.println(str);
        enviaDadosClientes("console", "->" + str);
    }
}
