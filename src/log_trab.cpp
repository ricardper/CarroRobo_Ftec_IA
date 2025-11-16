// log_trab.cpp
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
        String str2 = str + " " + valor + "\n";

        enviaDadosClientes("console", "->" + str + "_" + valor);
        Serial.println(str2);
    }
}

void logTrab(String str)
{
    if (DEBUG)
    {

        enviaDadosClientes("console", "->" + str);
        Serial.println(str);
    }
}
