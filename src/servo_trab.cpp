// servo_trab.cpp — VERSÃO FINAL OTIMIZADA (RADAR + HC-SR04)
//
// • Varredura horizontal perfeita para HC-SR04
// • Limite físico: 20° → 160° (evita chão, vibração, perda de eco)
// • Sensor com passo configurável (SERVO_MEDICAO_PASSO = 2 recomendado)
// • Envio WebSocket: radar,angulo,distancia
// • Baixo consumo de CPU e WS estável
//

#include "servo_trab.h"
#include "ultrassom_trab.h"

// ======================================================================
// Variáveis Globais
// ======================================================================

// Servo de direção (carro)
Servo servoDirecao;
volatile int servoUltimoValor = SERVO_CENTRO;

// Servo de radar (HC-SR04)
Servo servoMedicao;
bool medicaoLigado = true;
int posicaoMedicao = RADAR_MIN_ANGULO; // inicia no ângulo mínimo útil
int direcaoMedicao = 1;                // +1 sobe, -1 desce

// Controle de taxa de envio
static unsigned long ultimoEnvioRadar = 0;
const unsigned long INTERVALO_ENVIO_RADAR_MS = 70; // ~16 FPS máx.

// Limites reais do HC-SR04 para montagem horizontal

// ======================================================================
// Inicialização dos servos
// ======================================================================
int initServo()
{
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    // Servo da direção
    servoDirecao.attach(SERVO_DIRECAO_PIN);
    servoDirecao.write(SERVO_CENTRO);
    servoUltimoValor = SERVO_CENTRO;

    // Servo do radar
    servoMedicao.attach(SERVO_MEDICAO_PIN);
    posicaoMedicao = RADAR_MIN_ANGULO;
    servoMedicao.write(posicaoMedicao);

    logTrab("Servos inicializados");
    return 0;
}

// ======================================================================
// Direção
// ======================================================================

int setServoDirecao(int graus)
{
    if (graus < SERVO_DIRECAO_MIN)
        graus = SERVO_DIRECAO_MIN;
    if (graus > SERVO_DIRECAO_MAX)
        graus = SERVO_DIRECAO_MAX;

    if (graus == servoUltimoValor)
        return 0;

    int antes = servoUltimoValor;
    servoDirecao.write(graus);
    servoUltimoValor = graus;

    enviaDadosClientes("Direcao", graus);
    logTrab("Direcao antes " + String(antes) + " agora " + String(graus));

    return 0;
}

void addDirecao() { setServoDirecao(servoUltimoValor + SERVO_ADICAO_SUBTRACAO); }
void subDirecao() { setServoDirecao(servoUltimoValor - SERVO_ADICAO_SUBTRACAO); }
int getDirecaoGraus() { return servoDirecao.read(); }

// ======================================================================
// Radar / Medição Ultrassônica
// ======================================================================

static void enviaRadarLimitado(int angulo)
{
    unsigned long agora = millis();
    if (agora - ultimoEnvioRadar < INTERVALO_ENVIO_RADAR_MS)
        return;

    ultimoEnvioRadar = agora;

    float dist = lerDistanciaCm(); // retorna -1 se sem eco
    String payload = String(angulo) + "," + String(dist, 1);

    enviaDadosClientes("radar", payload);
}

/*
 * Movimento horizontal:
 *   20° → 160° → 20° → 160° ...
 *   Evita piso, teto, e evita puxão dos cabos
 */
void moveMedicao()
{
    if (!medicaoLigado)
        return;

    posicaoMedicao += direcaoMedicao * SERVO_MEDICAO_PASSO;

    // Limite superior
    if (posicaoMedicao >= RADAR_MAX_ANGULO)
    {
        posicaoMedicao = RADAR_MAX_ANGULO;
        direcaoMedicao = -1;
    }
    // Limite inferior
    else if (posicaoMedicao <= RADAR_MIN_ANGULO)
    {
        posicaoMedicao = RADAR_MIN_ANGULO;
        direcaoMedicao = 1;
    }

    // Move servo fisicamente
    servoMedicao.write(posicaoMedicao);

    // Envia radar
    enviaRadarLimitado(posicaoMedicao);
}

void ligaDesligaMedicao(int valor)
{
    medicaoLigado = (valor == 1);
    logTrab(String("medicao ") + (medicaoLigado ? "ligada" : "desligada"));
}

int getPosicaoMedicao()
{
    return posicaoMedicao;
}
