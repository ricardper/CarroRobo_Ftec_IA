
// webControl_trab.cpp
//
// VERSÃO FINAL — CORRIGIDA, OTIMIZADA E COM TRIM()
// - Sem eco de WebSocket
// - Sem comandos desconhecidos
// - Sem logs duplicados
// - UTF-8 funcionando
// - Fila WebSocket estável (sem "Too many messages queued")
//

#include "webControl_trab.h"

// Instâncias globais
static AsyncWebServer server(PORT_HTTP);
static AsyncWebSocket ws("/Carro");

// Estado (caso queira telemetria depois)
volatile int DirecaoLida = SERVO_CENTRO;
volatile int velocidadeLida = 0;

// ========================================================================
// EVENTO DO WEBSOCKET
// ========================================================================
static void onWsEvent(AsyncWebSocket *server,
                      AsyncWebSocketClient *client,
                      AwsEventType type,
                      void *arg,
                      uint8_t *data,
                      size_t len)
{
    switch (type)
    {
    // Cliente conectou
    case WS_EVT_CONNECT:
        Serial.println("Cliente WS conectado: " + String(client->id()));
        break;

    // Cliente desconectou
    case WS_EVT_DISCONNECT:
        Serial.println("Cliente WS desconectado: " + String(client->id()));
        break;

    // Mensagem recebida
    case WS_EVT_DATA:
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;

        // Só processa mensagens completas e de texto
        if (!(info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT))
            return;

        // Monta a mensagem
        String msg;
        msg.reserve(len);
        for (size_t i = 0; i < len; i++)
            msg += (char)data[i];

        msg.trim(); // remove \r, \n, \0 e espaços extras

        int commaIndex = msg.indexOf(',');
        if (commaIndex == -1)
        {
            logTrab("Comando WS inválido: " + msg);
            return;
        }

        // Separa comando e valor
        String comando = msg.substring(0, commaIndex);
        String valorStr = msg.substring(commaIndex + 1);

        comando.trim();
        valorStr.trim();

        int valor = valorStr.toInt();

        // ===============================================================
        // COMANDOS DO CARRO
        // ===============================================================

        if (comando == "parar")
        {
            deslMotor();
        }
        else if (comando == "frente")
        {
            addVelocidade();
        }
        else if (comando == "traz")
        {
            subVelocidade();
        }
        else if (comando == "esquerda")
        {
            subDirecao();
        }
        else if (comando == "direita")
        {
            addDirecao();
        }

        // ------------------- SLIDER DIREÇÃO -----------------------
        else if (comando == "Direcao")
        {
            if (valor >= 1 && valor <= 180)
                setServoDirecao(valor);
        }

        // ------------------- SLIDER VELOCIDADE -------------------
        else if (comando == "Velocidade")
        {
            if (valor >= -100 && valor <= 100)
                moverMotor(valor);
        }

        // ------------------- CONTROLE DO RADAR --------------------
        else if (comando == "medicao")
        {
            ligaDesligaMedicao(valor);
        }

        // ------------------- CONTROLE DE LOG/DEBUG ----------------
        else if (comando == "debug")
        {
            ligaDesligaDebug(valor);
        }

        // ------------------- NOVO: MODO ANDAR SOZINHO -------------
        else if (comando == "auto")
        {
            // Aqui você integra com sua biblioteca de "andar sozinho"
            // ligaDesligaMedicao(valor);
            ligaDesligaAutonomo(valor);
            logTrab(String("andar sozinho: ") + (valor == 1 ? "LIGADO" : "DESLIGADO"));
        }
        else if (comando == "recuodireita" || comando == "Recuodireita" || comando == "Rd")
        {
            macroRecuoCurvaDireita(valor);
            logTrab(" Recuo para a direita");
        }
        else if (comando == "recuoesquerda" || comando == "Recuoesquerda" || comando == "Re")
        {
            macroRecuoCurvaEsquerda(valor);
            logTrab(" Recuo para a esquerda");
        }
        else if (comando == "curvaesquerda" || comando == "Curvaesquerda" || comando == "Ce")
        {

            macroCurvaEsquerda(valor);
            logTrab(" curva a esquerda");
        }
        else if (comando == "curvadireita" || comando == "Curva direita" || comando == "Cd")
        {

            macroCurvaDireita(valor);
            logTrab(" curva a direita");
        }

        // ------------------- DESCONHECIDO -------------------------
        else
        {
            logTrab("Comando WS desconhecido: " + comando);
        }
    }
    break;
    }

} // fim do onWsEvent

// ========================================================================
// INICIALIZAÇÃO DO SERVIDOR
// ========================================================================
void initWebServer()
{
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200,
                              "text/html; charset=utf-8",
                              htmlHomePage); });

    server.begin();
    Serial.printf("Servidor AsyncWebServer iniciado na porta %d\n", PORT_HTTP);
}

// ========================================================================
// LIMPA CLIENTES INATIVOS (evita fila cheia do WS)
// ========================================================================
void cleanupWebClients()
{
    ws.cleanupClients();
}

// ========================================================================
// ENVIA DADOS PARA TODOS OS CLIENTES (INT)
// ========================================================================
void enviaDadosClientes(String chave, int valor)
{
    if (!ws.availableForWriteAll())
        return; // evita "Too many messages queued"

    ws.textAll(chave + "," + String(valor));
}

// ========================================================================
// ENVIA DADOS PARA TODOS OS CLIENTES (STRING)
// ========================================================================
void enviaDadosClientes(String chave, String dado)
{
    if (!ws.availableForWriteAll())
        return;

    ws.textAll(chave + "," + dado);
}
