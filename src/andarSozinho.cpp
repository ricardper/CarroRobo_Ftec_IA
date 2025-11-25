// src/andarSozinho.cpp
#include "andarSozinho.h"

// ===================== CONFIGURAÇÕES ==========================
bool modoAutonomoAtivo = false;

// thresholds / tempos (ajuste se quiser)
const int DISTANCIA_MINIMA = 50; // cm (ultrassom) -> detectar obstáculo
const int VELOCIDADE_PADRAO = 40;
const int TEMPO_CURVA_SUAVE = 250; // curva curta (ms)
const int TEMPO_CURVA_LONGA = 800; // curva longa "C" (ms) -- executa a manobra de desvio
const int TEMPO_RECUO = 300;       // ré curto (ms)
const int TEMPO_REAVALIACAO = 200; // espera antes de ler ultrassom após ação

const int ANGULO_ESQUERDA = 50;
const int ANGULO_DIREITA = 105;
const int ANGULO_CENTRO = SERVO_CENTRO;

const int NUM_MEDIDAS = 3; // mesmo do resto do projeto

// Setores (usados para análise de melhor lado)
const int ANG_ESQ_MIN = 10;
const int ANG_ESQ_MAX = 70;
const int ANG_FRT_MIN = 70;
const int ANG_FRT_MAX = 110;
const int ANG_DIR_MIN = 110;
const int ANG_DIR_MAX = 170;

const unsigned long COOLDOWN_ESCAPE = 1200; // evita executar escape várias vezes rápido

// ===================== BUFFERS ================================
float setorEsquerdo[NUM_MEDIDAS];
float setorFrontal[NUM_MEDIDAS];
float setorDireito[NUM_MEDIDAS];

int indiceEsq = 0, indiceFrente = 0, indiceDir = 0;

// ===================== ESTADO ==================================
enum Lado
{
    LADO_ESQ,
    LADO_DIR,
    LADO_INDEFINIDO
};
Lado ultimoLadoCurva = LADO_ESQ;

unsigned long tempoUltimoEscape = 0;
int contadorTravado = 0;

// Máquina de estados não-bloqueante
enum EstadoAuto
{
    AUTO_DRIVE,       // andar reto
    AUTO_ANALISA,     // parou, analisa melhores lados
    AUTO_CURVA_LONGA, // executa curva longa ("C")
    AUTO_VERIFICA,    // reavalia com ultrassom
    AUTO_RECUA,       // dá ré (se necessário)
    AUTO_FALLBACK     // fallback: tenta o outro lado / ré maior
};

static EstadoAuto estado = AUTO_DRIVE;
static unsigned long tInicioAcao = 0;
static int tentativaAtual = 0;
const int MAX_TENTATIVAS = 3; // evita loop infinito

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

// Decide melhor lado olhando média dos setores -> preferir lado com maior distancia média
Lado escolherMelhorLado(float dEsq, float dDir)
{
    if (dEsq > dDir + 5)
        return LADO_ESQ; // margem pequena para estabilidade
    if (dDir > dEsq + 5)
        return LADO_DIR;
    return LADO_INDEFINIDO; // se forem parecidos
}

// ===================== INICIALIZAÇÃO ============================
void initAndarSozinho()
{
    for (int i = 0; i < NUM_MEDIDAS; i++)
        setorEsquerdo[i] = setorFrontal[i] = setorDireito[i] = 200;

    indiceEsq = indiceFrente = indiceDir = 0;
    tempoUltimoEscape = 0;
    contadorTravado = 0;
    estado = AUTO_DRIVE;
    tInicioAcao = millis();
    tentativaAtual = 0;
}

// ===================== AÇÕES ====================================

void iniciarCurvaLonga(Lado lado)
{
    // marca último lado para usar no escape, e posiciona servo no lado escolhido
    if (lado == LADO_ESQ)
    {
        ultimoLadoCurva = LADO_ESQ;
        setServoDirecao(ANGULO_ESQUERDA);
    }
    else
    {
        ultimoLadoCurva = LADO_DIR;
        setServoDirecao(ANGULO_DIREITA);
    }
    moverMotor(VELOCIDADE_PADRAO);
    tInicioAcao = millis();
}

void iniciarRecuoCurto()
{
    moverMotor(-VELOCIDADE_PADRAO);
    tInicioAcao = millis();
}

void pararECentralizar()
{
    moverMotor(0);
    setServoDirecao(ANGULO_CENTRO);
}

// ===================== LOOP PRINCIPAL ===========================

void andarSozinhoLoop()
{
    // ATUALIZA SETORES (baseado no radar/laser — continua funcionando normalmente)
    int angulo = getPosicaoMedicao();
    float leituraLaser = lerDistanciaCm();
    atualizarSetores(leituraLaser, angulo);

    // DETECÇÃO PRINCIPAL: usa o ULTRASSOM como detector frontal
    float distUltra = lerDistanciaCmUltrasonico();

    unsigned long agora = millis();

    // Se estiver em cooldown após um escape, evita reentrar
    bool emCooldown = (agora - tempoUltimoEscape < COOLDOWN_ESCAPE);

    switch (estado)
    {
    case AUTO_DRIVE:
        // Andar reto por padrão
        moverMotor(VELOCIDADE_PADRAO);
        setServoDirecao(ANGULO_CENTRO);

        // Se ultrassom detectar obstáculo, para e passa para análise
        if (distUltra > 0 && distUltra <= DISTANCIA_MINIMA && !emCooldown)
        {
            pararECentralizar();
            estado = AUTO_ANALISA;
            tInicioAcao = agora;
            tentativaAtual = 0;
        }
        break;

    case AUTO_ANALISA:
        // Pausa rápida para garantir leituras estáveis
        if (agora - tInicioAcao < 50)
            break;

        // Calcula médias dos setores (laser) para decidir melhor lado
        {
            float dEsq = calcularMedia(setorEsquerdo);
            float dDir = calcularMedia(setorDireito);

            Lado melhor = escolherMelhorLado(dEsq, dDir);

            // Se não tiver uma diferença clara, escolhe lado oposto ao último ou tenta ambos
            if (melhor == LADO_INDEFINIDO)
            {
                // tenta o lado menos usado (inverte o último)
                melhor = (ultimoLadoCurva == LADO_ESQ) ? LADO_DIR : LADO_ESQ;
            }

            // inicia curva longa para o lado escolhido
            iniciarCurvaLonga(melhor);
            estado = AUTO_CURVA_LONGA;
            tInicioAcao = agora;
        }
        break;

    case AUTO_CURVA_LONGA:
        // Executa a curva por TEMPO_CURVA_LONGA ms
        if (agora - tInicioAcao < TEMPO_CURVA_LONGA)
        {
            // manter movimento e direção (já definido em iniciarCurvaLonga)
            break;
        }
        else
        {
            // terminou curva longa — agora verifica se ainda há obstáculo
            moverMotor(0); // pausa rápido
            tInicioAcao = agora;
            estado = AUTO_VERIFICA;
        }
        break;

    case AUTO_VERIFICA:
        // espera um pouco para estabilidade e lê ultrassom
        if (agora - tInicioAcao < TEMPO_REAVALIACAO)
            break;

        distUltra = lerDistanciaCmUltrasonico();

        // se limpou o caminho -> retomar em frente
        if (distUltra < 0 || distUltra > DISTANCIA_MINIMA)
        {
            // sucesso: centraliza e segue
            setServoDirecao(ANGULO_CENTRO);
            moverMotor(VELOCIDADE_PADRAO);
            estado = AUTO_DRIVE;
            tempoUltimoEscape = agora;
            tentativaAtual = 0;
        }
        else
        {
            // ainda obstruído -> incrementa tentativa e tenta "ré + curva oposta" ou repetir
            tentativaAtual++;
            if (tentativaAtual <= MAX_TENTATIVAS)
            {
                // dá ré curto e tenta o outro lado
                iniciarRecuoCurto();
                estado = AUTO_RECUA;
                tInicioAcao = agora;
            }
            else
            {
                // muitas tentativas -> fallback (ré maior e virar 180 ou para segurança)
                estado = AUTO_FALLBACK;
                tInicioAcao = agora;
            }
        }
        break;

    case AUTO_RECUA:
        if (agora - tInicioAcao < TEMPO_RECUO)
        {
            // ainda ré
            break;
        }
        else
        {
            // terminou ré -> tenta curva no outro lado (oposto ao ultimo)
            Lado oposto = (ultimoLadoCurva == LADO_ESQ) ? LADO_DIR : LADO_ESQ;
            iniciarCurvaLonga(oposto);
            estado = AUTO_CURVA_LONGA;
            tInicioAcao = agora;
        }
        break;

    case AUTO_FALLBACK:
        // Procedimento de segurança/recuperação: faz ré maior e centraliza, depois tenta andar devagar
        if (agora - tInicioAcao < (unsigned long)(TEMPO_RECUO * 3))
        {
            moverMotor(-VELOCIDADE_PADRAO);
        }
        else
        {
            // Depois de ré maior tenta girar levemente para o lado oposto e avançar lentamente
            setServoDirecao((ultimoLadoCurva == LADO_ESQ) ? ANGULO_DIREITA : ANGULO_ESQUERDA);
            moverMotor(VELOCIDADE_PADRAO / 2);
            delay(150); // pequena manobra controlada
            setServoDirecao(ANGULO_CENTRO);
            moverMotor(VELOCIDADE_PADRAO / 2);
            estado = AUTO_DRIVE;
            tempoUltimoEscape = agora;
            tentativaAtual = 0;
        }
        break;
    } // fim switch
}

// Dispara/desliga modo autônomo
void ligaDesligaAutonomo(int v)
{
    modoAutonomoAtivo = (v == 1);
    if (!modoAutonomoAtivo)
    {
        moverMotor(0);
        setServoDirecao(SERVO_CENTRO);
        estado = AUTO_DRIVE;
    }
    else
    {
        // reset estados quando ligar
        initAndarSozinho();
    }
}
