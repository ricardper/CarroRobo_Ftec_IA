#include "andarSozinho.h"

bool modoAutonomoAtivo = false;

// ===================== CONFIGURAÇÕES ==========================

// distâncias
const int DISTANCIA_MINIMA = 30; // cm para desviar
const int NUM_MEDIDAS = 3;       // filtro de média

// velocidades e movimentos
const int VELOCIDADE_PADRAO = 70;
const int TEMPO_CURVA_SUAVE = 300;  // ms
const int TEMPO_CURVA_ESCAPE = 700; // ms
const int TEMPO_RECUO = 400;        // ms

// ângulos
const int ANGULO_ESQUERDA = 50;
const int ANGULO_DIREITA = 120;
const int ANGULO_CENTRO = SERVO_CENTRO;

// cooldown após escape
const unsigned long COOLDOWN_ESCAPE = 1200;

// ===================== BUFFERS DE SETOR ========================

float setorEsquerdo[NUM_MEDIDAS];
float setorFrontal[NUM_MEDIDAS];
float setorDireito[NUM_MEDIDAS];

int indiceEsq = 0, indiceFrente = 0, indiceDir = 0;

// ===================== ESTADO GLOBAL ===========================

enum Lado
{
    LADO_ESQ = 0,
    LADO_DIR = 1
};
Lado ultimoLadoCurva = LADO_ESQ;

unsigned long tempoUltimoEscape = 0;
int contadorTravado = 0;

// ===================== FUNÇÕES AUXILIARES ======================

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

    if (angulo >= 0 && angulo <= 60)
    {
        setorEsquerdo[indiceEsq] = distancia;
        indiceEsq = (indiceEsq + 1) % NUM_MEDIDAS;
    }
    else if (angulo >= 70 && angulo <= 110)
    {
        setorFrontal[indiceFrente] = distancia;
        indiceFrente = (indiceFrente + 1) % NUM_MEDIDAS;
    }
    else if (angulo >= 120 && angulo <= 180)
    {
        setorDireito[indiceDir] = distancia;
        indiceDir = (indiceDir + 1) % NUM_MEDIDAS;
    }
}

// ===================== MANOBRAS ================================

void curvaEsquerdaSuave()
{
    ultimoLadoCurva = LADO_ESQ;
    setServoDirecao(ANGULO_ESQUERDA);
    moverMotor(VELOCIDADE_PADRAO);
    delay(TEMPO_CURVA_SUAVE);
    setServoDirecao(ANGULO_CENTRO);
}

void curvaDireitaSuave()
{
    ultimoLadoCurva = LADO_DIR;
    setServoDirecao(ANGULO_DIREITA);
    moverMotor(VELOCIDADE_PADRAO);
    delay(TEMPO_CURVA_SUAVE);
    setServoDirecao(ANGULO_CENTRO);
}

// ===================== MANOBRA ANTICANTO =======================

void executarEscapeCanto()
{
    tempoUltimoEscape = millis();
    contadorTravado = 0;

    // 1) pequena pausa
    moverMotor(0);
    delay(100);

    // 2) recuar
    moverMotor(-60);
    delay(TEMPO_RECUO);

    // 3) curva longa para lado oposto
    if (ultimoLadoCurva == LADO_ESQ)
        setServoDirecao(ANGULO_DIREITA);
    else
        setServoDirecao(ANGULO_ESQUERDA);

    // 4) avançar em curva grande
    moverMotor(VELOCIDADE_PADRAO);
    delay(TEMPO_CURVA_ESCAPE);

    setServoDirecao(ANGULO_CENTRO);
}

// ===================== INICIALIZAÇÃO ============================

void initAndarSozinho()
{
    for (int i = 0; i < NUM_MEDIDAS; i++)
        setorEsquerdo[i] = setorFrontal[i] = setorDireito[i] = 200;

    indiceEsq = indiceFrente = indiceDir = 0;
    tempoUltimoEscape = 0;
    contadorTravado = 0;
}

// ===================== LOOP PRINCIPAL ===========================

void andarSozinhoLoop()
{
    int angulo = getPosicaoMedicao();
    float distancia = lerDistanciaCm();

    atualizarSetores(distancia, angulo);

    float dEsq = calcularMedia(setorEsquerdo);
    float dFrente = calcularMedia(setorFrontal);
    float dDir = calcularMedia(setorDireito);

    unsigned long agora = millis();
    bool emCooldown = (agora - tempoUltimoEscape < COOLDOWN_ESCAPE);

    // ---------- DETECTAR CANTO ----------
    if (!emCooldown)
    {
        if (dFrente < DISTANCIA_MINIMA && dEsq < DISTANCIA_MINIMA && dDir < DISTANCIA_MINIMA)
            contadorTravado++;
        else
            contadorTravado = 0;

        if (contadorTravado >= 5)
        {
            executarEscapeCanto();
            return;
        }
    }

    // ---------- MODO NORMAL ----------
    moverMotor(VELOCIDADE_PADRAO);

    if (dFrente < DISTANCIA_MINIMA)
    {
        if (dEsq > dDir)
            curvaEsquerdaSuave();
        else
            curvaDireitaSuave();
    }
}

void ligaDesligaAutonomo(int v)
{
    modoAutonomoAtivo = (v == 1);
    if (!modoAutonomoAtivo)
    {
        moverMotor(0); // parar imediatamente
        setServoDirecao(SERVO_CENTRO);
    }
}
