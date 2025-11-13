
#include "wifi_trab.h"

int initWifi()
{
    Serial.print("Conectando a ");
    Serial.println(SSID_WIFI);

    WiFi.begin(SSID_WIFI, PASSWORD_WIFI);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    printWifiConect();
    return 0;
}

void printWifiConect()
{
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}