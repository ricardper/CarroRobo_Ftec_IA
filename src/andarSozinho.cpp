#include "andarSozinho.h"

bool modoAutonomoAtivo = false;

// ===================== CONFIGURAÇÕES ==========================

const int DISTANCIA_MINIMA = 50;
const int VELOCIDADE_PADRAO = 40;
const int TEMPO_CURVA_SUAVE = 250;
const int TEMPO_RECUO = 250;
const int TEMPO_CURVA_ESCAPE = 450;

const int ANGULO_ESQUERDA = 50;
const int ANGULO_DIREITA = 105;
const int ANGULO_CENTRO = SERVO_CENTRO;

const int NUM_MEDIDAS = 3;

// Setores ajustados ao radar 10° → 170°
const int ANG_ESQ_MIN = 10;
const int ANG_ESQ_MAX = 70;
const int ANG_FRT_MIN = 70;
const int ANG_FRT_MAX = 110;
const int ANG_DIR_MIN = 110;
const int ANG_DIR_MAX = 170;

const unsigned long COOLDOWN_ESCAPE = 1200;

// ===================== BUFFERS ================================
float setorEsquerdo[NUM_MEDIDAS];
float setorFrontal[NUM_MEDIDAS];
float setorDireito[NUM_MEDIDAS];

int indiceEsq = 0, indiceFrente = 0, indiceDir = 0;

// ===================== ESTADO ==================================

enum Lado
{
    LADO_ESQ,
    LADO_DIR
};
Lado ultimoLadoCurva = LADO_ESQ;

unsigned long tempoUltimoEscape = 0;
int contadorTravado = 0;

// Estados não bloqueantes
enum EstadoAuto
{
    AUTO_AVANCAR,
    AUTO_CURVA_ESQ,
    AUTO_CURVA_DIR,
    AUTO_RECUO,
    AUTO_ESCAPE
};

EstadoAuto estadoAtual = AUTO_AVANCAR;
unsigned long tInicioAcao = 0;

// ===================== AUXILIARES =================================

float calcularMedia(float *buffer)
{
    float soma = 0;
    for (int i = 0; i < NUM_MEDIDAS; i++)
        soma += buffer[i];
    return soma / NUM_MEDIDAS;
}

void atualizarSetores(float distancia, int angulo)
{
    if (distancia < 0)
        distancia = 200;

    if (angulo >= ANG_ESQ_MIN && angulo <= ANG_ESQ_MAX)
    {
        setorEsquerdo[indiceEsq] = distancia;
        indiceEsq = (indiceEsq + 1) % NUM_MEDIDAS;
    }
    else if (angulo > ANG_FRT_MIN && angulo <= ANG_FRT_MAX)
    {
        setorFrontal[indiceFrente] = distancia;
        indiceFrente = (indiceFrente + 1) % NUM_MEDIDAS;
    }
    else if (angulo > ANG_DIR_MIN && angulo <= ANG_DIR_MAX)
    {
        setorDireito[indiceDir] = distancia;
        indiceDir = (indiceDir + 1) % NUM_MEDIDAS;
    }
}

// ===================== INICIALIZAÇÃO ===============================

void initAndarSozinho()
{
    for (int i = 0; i < NUM_MEDIDAS; i++)
        setorEsquerdo[i] = setorFrontal[i] = setorDireito[i] = 200;

    indiceEsq = indiceFrente = indiceDir = 0;
    tempoUltimoEscape = 0;
    contadorTravado = 0;
    estadoAtual = AUTO_AVANCAR;
    tInicioAcao = millis();
}

// ===================== ESTADOS NÃO BLOQUEANTES =====================

void tratarEstadoAvancar(float dEsq, float dFrente, float dDir)
{
    moverMotor(VELOCIDADE_PADRAO);
    setServoDirecao(ANGULO_CENTRO);

    if (dFrente < DISTANCIA_MINIMA)
    {
        if (dEsq > dDir)
        {
            estadoAtual = AUTO_CURVA_ESQ;
            tInicioAcao = millis();
        }
        else
        {
            estadoAtual = AUTO_CURVA_DIR;
            tInicioAcao = millis();
        }
    }
}

void tratarEstadoCurvaEsq()
{
    setServoDirecao(ANGULO_ESQUERDA);
    moverMotor(VELOCIDADE_PADRAO);

    if (millis() - tInicioAcao >= TEMPO_CURVA_SUAVE)
    {
        estadoAtual = AUTO_AVANCAR;
    }
}

void tratarEstadoCurvaDir()
{
    setServoDirecao(ANGULO_DIREITA);
    moverMotor(VELOCIDADE_PADRAO);

    if (millis() - tInicioAcao >= TEMPO_CURVA_SUAVE)
    {
        estadoAtual = AUTO_AVANCAR;
    }
}

void tratarEstadoRecuo()
{
    moverMotor(-VELOCIDADE_PADRAO);

    if (millis() - tInicioAcao >= TEMPO_RECUO)
    {
        if (ultimoLadoCurva == LADO_ESQ)
            estadoAtual = AUTO_CURVA_DIR;
        else
            estadoAtual = AUTO_CURVA_ESQ;

        tInicioAcao = millis();
    }
}

void tratarEstadoEscape()
{
    moverMotor(VELOCIDADE_PADRAO);
    setServoDirecao(ultimoLadoCurva == LADO_ESQ ? ANGULO_DIREITA : ANGULO_ESQUERDA);

    if (millis() - tInicioAcao >= TEMPO_CURVA_ESCAPE)
    {
        setServoDirecao(ANGULO_CENTRO);
        estadoAtual = AUTO_AVANCAR;
    }
}

// ===================== LOOP PRINCIPAL ==============================

void andarSozinhoLoop()
{
    int angulo = getPosicaoMedicao();
    float distancia = lerDistanciaCm();

    atualizarSetores(distancia, angulo);

    float dEsq = calcularMedia(setorEsquerdo);
    float dFrente = calcularMedia(setorFrontal);
    float dDir = calcularMedia(setorDireito);

    unsigned long agora = millis();

    // Detectar canto
    bool emCooldown = (agora - tempoUltimoEscape < COOLDOWN_ESCAPE);

    if (!emCooldown)
    {
        if (dFrente < DISTANCIA_MINIMA && dEsq < DISTANCIA_MINIMA && dDir < DISTANCIA_MINIMA)
            contadorTravado++;
        else
            contadorTravado = 0;

        if (contadorTravado >= 4)
        {
            estadoAtual = AUTO_RECUO;
            tInicioAcao = agora;
            tempoUltimoEscape = agora;
            contadorTravado = 0;
            return;
        }
    }

    switch (estadoAtual)
    {
    case AUTO_AVANCAR:
        tratarEstadoAvancar(dEsq, dFrente, dDir);
        break;
    case AUTO_CURVA_ESQ:
        tratarEstadoCurvaEsq();
        break;
    case AUTO_CURVA_DIR:
        tratarEstadoCurvaDir();
        break;
    case AUTO_RECUO:
        tratarEstadoRecuo();
        break;
    case AUTO_ESCAPE:
        tratarEstadoEscape();
        break;
    }
}

void ligaDesligaAutonomo(int v)
{
    modoAutonomoAtivo = (v == 1);

    if (!modoAutonomoAtivo)
    {
        moverMotor(0);
        setServoDirecao(SERVO_CENTRO);
        estadoAtual = AUTO_AVANCAR;
    }
}
