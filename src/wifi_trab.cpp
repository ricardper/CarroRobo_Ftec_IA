#include "wifi_trab.h"

// Função auxiliar para inicializar como Cliente
int initWifiClient()
{
    Serial.print("Conectando como Cliente a: ");
    Serial.println(CLIENT_SSID_WIFI);

    // Conecta à rede Wi-Fi usando as credenciais do modo Cliente
    WiFi.begin(CLIENT_SSID_WIFI, CLIENT_PASSWORD_WIFI);

    // Aguarda a conexão com timeout de 20 segundos (40 iterações de 500ms)
    int timeout_count = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        timeout_count++;

        // Adiciona um limite de tempo (timeout) para evitar loop infinito
        if (timeout_count > 40)
        {
            Serial.println("\nFalha na conexao no modo Cliente. Timeout.");
            return -1; // Retorna codigo de erro
        }
    }

    printWifiConect();
    return 0; // Sucesso
}

// Função auxiliar para inicializar como Access Point (AP)
int initWifiAP()
{
    Serial.print("Configurando como Ponto de Acesso (AP): ");
    Serial.println(AP_SSID_WIFI);

    // Define o ESP32 como Ponto de Acesso
    // O retorno e o número do canal do AP, ou 0 em erro
    bool result = WiFi.softAP(AP_SSID_WIFI, AP_PASSWORD_WIFI);

    if (!result)
    {
        Serial.println("Falha ao configurar AP!");
        return -2; // Retorna código de erro
    }

    printWifiConect();
    return 0; // Sucesso
}

// =================================================================
// FUNÇÃO PRINCIPAL REFATORADA
// =================================================================

int initWifi()
{
    WiFi.mode(WIFI_STA); // Inicializa a interface Wi-Fi como Station (Cliente)

// Verifica o modo de operação definido em config.h
#if WIFI_AP_TRAB == 1
                         // Modo AP (Access Point)
    return initWifiAP();
#else
                         // Modo Cliente (Station)
    return initWifiClient();
#endif
}

void printWifiConect()
{
    Serial.println("");

#if WIFI_AP_TRAB == 1
    Serial.println("AP configurado com sucesso.");
    Serial.print("SSID: ");
    Serial.println(AP_SSID_WIFI);
    Serial.print("IP address (Gateway): ");
    Serial.println(WiFi.softAPIP()); // Usa softAPIP para AP
#else
    Serial.println("WiFi conectado (Cliente).");
    Serial.print("SSID: ");
    Serial.println(CLIENT_SSID_WIFI);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); // Usa localIP para Cliente
#endif
}