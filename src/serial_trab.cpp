// serial_trab.cpp

#include "serial_trab.h"

String entradaSerial;

void lerComando()
{
    while (Serial.available() > 0)
    {
        char caractere = Serial.read();
        entradaSerial += caractere;

        if (caractere == '\n' || caractere == '\r')
        {
            processarComando(entradaSerial);
            entradaSerial = "";
        }
    }
}

void processarComando(String entrada)
{
    // remove \r, \n e espaços extras das pontas
    entrada.trim();

    int comandoIndex = entrada.indexOf(',');
    if (comandoIndex != -1)
    {
        String comando = entrada.substring(0, comandoIndex);
        String valorStr = entrada.substring(comandoIndex + 1);
        valorStr.trim(); // garante que não tem \r ou \n

        int valor = valorStr.toInt();

        // logTrab("-comando digitado -->" + comando + "," + valor + "<--");
        Serial.println("-comando digitado -->" + comando + "," + String(valor) + "<--");

        if (comando == "direcao")
        {
            // Direcao/Servo (1 a 180)
            if (valor >= 1 && valor <= 180)
            {
                setServoDirecao(valor);
            }
        }
        else if (comando == "velocidade")
        {
            // Aceleracao/Motor (0 a 100)
            if (valor >= 0 && valor <= 100)
            {
                moverMotor(valor);
            }
        }
        else if (comando == "medicao")
        {
            ligaDesligaMedicao(valor);
        }
        else if (comando == "debug")
        {
            ligaDesligaDebug(valor);
        }
    }
}
