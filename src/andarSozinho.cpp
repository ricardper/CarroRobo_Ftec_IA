#include "andarSozinho.h"

// comportamento e tempos (você já tem valores parecidos em config.h)
bool modoAutonomoAtivo = false;

const int DISTANCIA_MINIMA = 50; // threshold para o ultrassom disparar análise
const int VELOCIDADE_PADRAO = 40;
const int TEMPO_CURVA_SUAVE = 250;  // curva rápida (ms)
const int TEMPO_RECUO = 300;        // recuo (ms) — usado na curva longa
const int TEMPO_CURVA_ESCAPE = 700; // curva longa C (ms)

// ângulos (use os definidos em config.h se preferir)
const int ANGULO_ESQUERDA = 50;
const int ANGULO_DIREITA = 105;
const int ANGULO_CENTRO = SERVO_CENTRO;

const int NUM_MEDIDAS = 3;

// setores de laser — compatível com seu radar
const int ANG_ESQ_MIN = 10;
const int ANG_ESQ_MAX = 70;
const int ANG_FRT_MIN = 70;
const int ANG_FRT_MAX = 110;
const int ANG_DIR_MIN = 110;
const int ANG_DIR_MAX = 170;

// tempo que fica parado analisando (ms) — você pediu 5 s
const unsigned long TEMPO_ANALISE_MS = 5000UL;

// cooldown para evitar re-entradas rápidas após escape
const unsigned long COOLDOWN_ESCAPE = 1200UL;

// buffers de leitura do laser (setores)
float setorEsquerdo[NUM_MEDIDAS];
float setorFrontal[NUM_MEDIDAS];
float setorDireito[NUM_MEDIDAS];
int indiceEsq = 0, indiceFrente = 0, indiceDir = 0;

// estado e variáveis de controle
enum Lado
{
    LADO_ESQ,
    LADO_DIR
};
Lado ultimoLadoCurva = LADO_ESQ;

unsigned long tempoUltimoEscape = 0;
int contadorTravado = 0;

// Estados não bloqueantes (ampliado)
enum EstadoAuto
{
    AUTO_AVANCAR,
    AUTO_ANALISAR,    // parado, esperando laser completar scan (5s)
    AUTO_DECIDIR,     // decidir melhor lado após análise
    AUTO_CURVA_LONGA, // executar curva longa tipo C (recuo + virar)
    AUTO_REANALISAR,  // após curva, reavaliar se caminho foi liberado
    AUTO_AVISO        // estado de fallback (opcional)
};

EstadoAuto estadoAtual = AUTO_AVANCAR;
unsigned long tInicioAcao = 0;

// =================================================
// Auxiliares
// =================================================
float calcularMedia(float *buffer)
{
    float soma = 0;
    for (int i = 0; i < NUM_MEDIDAS; i++)
        soma += buffer[i];
    return soma / NUM_MEDIDAS;
}

void limparMemoriaSetores()
{
    for (int i = 0; i < NUM_MEDIDAS; i++)
    {
        setorEsquerdo[i] = setorFrontal[i] = setorDireito[i] = 200.0f;
    }
    indiceEsq = indiceFrente = indiceDir = 0;
}

// atualiza buffers por setor com a leitura do laser
void atualizarSetores(float distancia, int angulo)
{
    if (distancia < 0)
        distancia = 200.0f;

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

// =================================================
// Estados/ações
// =================================================
void iniciarAnalise()
{
    // para o motor e começa a contar TEMPO_ANALISE_MS sem bloquear
    moverMotor(0);
    estadoAtual = AUTO_ANALISAR;
    tInicioAcao = millis();
    // não limpamos setores aqui: queremos que o laser preencha
}

void decidirMelhorLado()
{
    // calcula médias e escolhe lado com maior espaço
    float dEsq = calcularMedia(setorEsquerdo);
    float dDir = calcularMedia(setorDireito);

    // se estiver indeciso (próximo), usa último lado invertido
    if (dEsq == dDir)
    {
        // inverte escolha para evitar ficar sempre no mesmo lado
        ultimoLadoCurva = (ultimoLadoCurva == LADO_ESQ) ? LADO_DIR : LADO_ESQ;
    }
    else if (dEsq > dDir)
    {
        ultimoLadoCurva = LADO_ESQ;
    }
    else
    {
        ultimoLadoCurva = LADO_DIR;
    }

    estadoAtual = AUTO_CURVA_LONGA;
    tInicioAcao = millis();
}

// executa a curva longa tipo "C": pequena pausa, recuo, virar para lado oposto,
// avançar em curva longa (tudo com tempos definidos)
void executarCurvaLonga()
{
    // marca cooldown para evitar retriggers
    tempoUltimoEscape = millis();

    // 1) pausa curta
    moverMotor(0);

    // 2) recuar por TEMPO_RECUO (não-bloqueante: fazemos via estado e temporização)
    // para simplificar, usaremos um comportamento em etapas:
    // - durante a primeira janela TEMPO_RECUO: recuar
    // - depois: girar para o lado escolhido e avançar por TEMPO_CURVA_ESCAPE
    // Implementação: no AUTO_CURVA_LONGA tratamos as sub-etapas
}

// =================================================
// Funções que tratam cada estado no loop principal
// =================================================

void tratarEstadoAvancar(float dEsq, float dFrente, float dDir)
{
    moverMotor(VELOCIDADE_PADRAO);
    setServoDirecao(ANGULO_CENTRO);

    // gatilho inicial: ULTRASSOM detectou obstáculo próximo
    float dUltra = lerDistanciaCmUltrasonico();
    if (dUltra > 0 && dUltra <= DISTANCIA_MINIMA)
    {
        // começa o processo de análise/evitação
        iniciarAnalise();
        return;
    }

    // fallback: se laser frontal indicar obstáculo muito perto (redundância)
    if (dFrente < (DISTANCIA_MINIMA - 10))
    {
        iniciarAnalise();
        return;
    }
}

void tratarEstadoAnalizar()
{
    // permanece parado mas deixa o radar/laser rodando (não bloqueante)
    moverMotor(0);

    if (millis() - tInicioAcao >= TEMPO_ANALISE_MS)
    {
        // passou 5s de análise → decide melhor lado
        estadoAtual = AUTO_DECIDIR;
    }
}

void tratarEstadoDecidir()
{
    // decide o melhor lado com base nas médias do laser
    decidirMelhorLado();
    // prepara sub-etapa: start recuo
    // usaremos "tInicioAcao" como referência da sub-etapa recuo
    tInicioAcao = millis();
}

void tratarEstadoCurvaLonga()
{
    unsigned long fase = millis() - tInicioAcao;

    if (fase < 100)
    {
        // pequena pausa inicial
        moverMotor(0);
    }
    else if (fase < (100 + TEMPO_RECUO))
    {
        // recuo
        moverMotor(-VELOCIDADE_PADRAO);
    }
    else if (fase < (100 + TEMPO_RECUO + TEMPO_CURVA_ESCAPE))
    {
        // virar para lado oposto e avançar em curva longa
        if (ultimoLadoCurva == LADO_ESQ)
        {
            // se escolhemos esquerda, virar DIREITA (curva em C abre para frente)
            setServoDirecao(ANGULO_DIREITA);
        }
        else
        {
            setServoDirecao(ANGULO_ESQUERDA);
        }
        moverMotor(VELOCIDADE_PADRAO);
    }
    else
    {
        // fim da curva longa -> centraliza e vai para reanálise
        setServoDirecao(ANGULO_CENTRO);
        moverMotor(0);
        estadoAtual = AUTO_REANALISAR;
        tInicioAcao = millis();
        // pequena limpeza temporária dos índices (mantemos leituras recentes até reanálise)
    }
}

void tratarEstadoReanalisar()
{
    // Reanalisa frontal usando laser (e opcionalmente ultrassom).
    // Vamos esperar uma pequena janela (ex: 300 ms) para permitir micro-movimento
    // e leitura do laser após a curva.
    const unsigned long JANELA_REANALISE_MS = 300UL;

    if (millis() - tInicioAcao < JANELA_REANALISE_MS)
    {
        // espera
        moverMotor(0);
        return;
    }

    // calcula médias atuais
    float dFrente = calcularMedia(setorFrontal);

    // e também verifica ultrassom frontal como redundância
    float dUltra = lerDistanciaCmUltrasonico();

    bool liberadoLaser = (dFrente >= DISTANCIA_MINIMA);
    bool liberadoUltra = (dUltra < 0) || (dUltra > DISTANCIA_MINIMA);

    if (liberadoLaser && liberadoUltra)
    {
        // sucesso: limpamos memória para evitar dados antigos e voltamos a avançar
        limparMemoriaSetores();
        estadoAtual = AUTO_AVANCAR;
        moverMotor(VELOCIDADE_PADRAO);
    }
    else
    {
        // ainda bloqueado: recomeça análise (para não travar em loop, atualiza cooldown)
        estadoAtual = AUTO_ANALISAR;
        tInicioAcao = millis();
        tempoUltimoEscape = millis();
    }
}

// =================================================
// Inicialização e loop público
// =================================================

void initAndarSozinho()
{
    limparMemoriaSetores();
    tempoUltimoEscape = 0;
    contadorTravado = 0;
    estadoAtual = AUTO_AVANCAR;
    tInicioAcao = millis();
}

void andarSozinhoLoop()
{
    // O main() já chama moveMedicao() e lerDistanciaCm() via radar; aqui apenas usamos
    // a leitura laser atual (lerDistanciaCm()) e posição do servo para alimentar setores.
    int angulo = getPosicaoMedicao();
    float distanciaLaser = lerDistanciaCm();            // leitura do laser (pode ser -1)
    float distanciaUltra = lerDistanciaCmUltrasonico(); // leitura do ultrassom (gatilho)

    // atualiza setores com a leitura do laser (para decisão)
    atualizarSetores(distanciaLaser, angulo);

    // médias para uso geral (apenas cálculo rápido)
    float dEsq = calcularMedia(setorEsquerdo);
    float dFrente = calcularMedia(setorFrontal);
    float dDir = calcularMedia(setorDireito);

    // Evita triggers se acabamos de escapar (cooldown)
    unsigned long agora = millis();
    if (agora - tempoUltimoEscape < COOLDOWN_ESCAPE)
    {
        // ainda em cooldown: podemos permanecer em AUTO_REANALISAR ou voltar a avançar
        // mas deixamos o estado atual seguir seu fluxo
    }

    // Estado machine
    switch (estadoAtual)
    {
    case AUTO_AVANCAR:
        tratarEstadoAvancar(dEsq, dFrente, dDir);
        break;

    case AUTO_ANALISAR:
        tratarEstadoAnalizar();
        break;

    case AUTO_DECIDIR:
        tratarEstadoDecidir();
        break;

    case AUTO_CURVA_LONGA:
        tratarEstadoCurvaLonga();
        break;

    case AUTO_REANALISAR:
        tratarEstadoReanalisar();
        break;

    default:
        // fallback — volte a avançar
        estadoAtual = AUTO_AVANCAR;
        break;
    }
}

// Liga / desliga autonomia
void ligaDesligaAutonomo(int v)
{
    modoAutonomoAtivo = (v == 1);

    if (!modoAutonomoAtivo)
    {
        moverMotor(0);
        setServoDirecao(SERVO_CENTRO);
        estadoAtual = AUTO_AVANCAR;
    }
    else
    {
        // quando ligar, limpa memória pra começar limpo
        limparMemoriaSetores();
        estadoAtual = AUTO_AVANCAR;
    }
}
