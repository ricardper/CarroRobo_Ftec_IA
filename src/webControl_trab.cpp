
#include "webControl_trab.h"

// Instâncias globais do servidor e WebSocket (static é seguro aqui)
static AsyncWebServer server(PORT_HTTP);
static AsyncWebSocket ws("/Carro");

// Variáveis de estado do carro (volatile para uso seguro entre interrupções/tarefas assíncronas)
volatile int DirecaoLida = 81;   // Centro
volatile int velocidadeLida = 0; // Parado

// Função de Callback para Gerenciar Eventos do WebSocket
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len)
{

    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("Cliente WS conectado: %u\n", client->id());
        break;

    case WS_EVT_DISCONNECT:
        Serial.printf("Cliente WS desconectado: %u\n", client->id());
        break;
    case WS_EVT_DATA:
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
        {

            String msg = (char *)data;
            // O seu JS envia "Key,Value" (ex: "Speed,150" ou "MoveCar,1")
            int commaIndex = msg.indexOf(',');
            if (commaIndex != -1)
            {
                String comando = msg.substring(0, commaIndex);
                String valorStr = msg.substring(commaIndex + 1);
                int valor = valorStr.toInt();

                if (comando == "parar")
                {
                    logTrab("deslMotor");
                    deslMotor();
                }
                else if (comando == "frente")
                {
                    logTrab("addVelocidade");
                    addVelocidade();
                }
                else if (comando == "traz")
                {

                    logTrab("subVelocidade");
                    subVelocidade();
                }
                else if (comando == "esquerda")
                {

                    logTrab("subdirecao");
                    subDirecao();
                }
                else if (comando == "direita")
                {

                    logTrab("addDirecao");
                    addDirecao();
                }

                else if (comando == "Direcao")
                {

                    if (valor >= 1 && valor <= 180)
                    {

                        setServoDirecao(valor);
                    }
                }
                else if (comando == "Velocidade")
                {

                    if (valor >= -100 && valor <= 100)
                    {

                        moverMotor(valor);
                    }
                }
            }
        }
        break;
    }
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void initWebServer()
{
    // Definir a função que gerencia eventos WebSocket
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    // Rota
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", htmlHomePage); });

    server.begin();
    Serial.printf("Servidor AsyncWebServer iniciado na porta %d\n", PORT_HTTP);
}

// Limpeza de clientes inativos
void cleanupWebClients()
{
    ws.cleanupClients();
}

void enviaDadosClientes(String str, int valor)
{

    String mensagem = str + "," + String(valor);

    ws.textAll(mensagem);
}
void enviaDadosClientes(String str, String dado)
{

    String mensagem = str + "," + String(dado);

    ws.textAll(mensagem);
}