#include "moverSozinho.h"
int recuo = 0;
unsigned long tempoAgora = 0;
unsigned long ultimoTempo = 0;

int temposAposManobra = 800;
// unsigned long ultimoRadar = 0;

int direcaoAnterior; // 0 para direita 1 para esquerda

void initMoverSozinho()
{
    direcaoAnterior = 1;
}

void moverSozinho(float distUlt)
{
    tempoAgora = millis();

    if (tempoAgora - ultimoTempo < _INTERVALO_TEMPO_FRENTE)

        if (distUlt > _DISTANCIA_MINIMA_)
        {
            // ultimoTempo = millis();
            moverFrente(_VELOCIDADE_FRENTE_);
            return;
        }
    logTrab("objeto detectado", (int)distUlt);
    deslMotor();

    ultimoTempo = millis();

    if (distUlt < _DISTANCIA_MINIMA_RECUO_)
    {

        curvarRecuo(distUlt);
        return;
    }
    if (distUlt < _DISTANCIA_MINIMA_)
    {

        curva(distUlt);
        return;
    }
    // inicio sensor laiser
    if (distanciaFrente <= 7) /// recua bem para traz evita colizao
    {
        deslMotor();

        moverRe(_VELOCIDADE_CURVA_);

        deslMotor();

        distanciaFrente = 200;
        return;
    }

    if (distanciaDireita <= _DISTANCIA_MINIMA_RECUO_)
    {
        curva(distanciaDireita);

        distanciaDireita = 200;
        return;
    }
    else if (distanciaEsquerda <= _DISTANCIA_MINIMA_RECUO_)
    {
        curva(distanciaEsquerda);
        distanciaEsquerda = 200;

        return;
    }

    // moverFrente(_VELOCIDADE_FRENTE_);
}

int medirFrente()
{

    return -1;
}

void macroRecuoCurvaDireita(int tempoMilisegundos)
{
    int duracao = _TEMPO_RECUO_;
    if (tempoMilisegundos > 0)
    {
        duracao = tempoMilisegundos;
    }

    deslMotor();
    setServoDirecao(SERVO_DIRECAO_MIN);
    delay(200);
    moverRe(_VELOCIDADE_RECUO_);
    delay(duracao);
    deslMotor();
    macroCurvaDireita(duracao - 200);
    delay(200);
    logTrab("mover recuo direita");

    // macroCurvaEsquerda(duracao);
}

void macroRecuoCurvaEsquerda(int tempoMilisegundos)
{
    int duracao = _TEMPO_RECUO_;
    if (tempoMilisegundos > 0)
    {
        duracao = tempoMilisegundos;
    }
    deslMotor();
    setServoDirecao(SERVO_DIRECAO_MAX);
    delay(200);
    moverRe(_VELOCIDADE_RECUO_);
    delay(duracao);
    deslMotor();
    macroCurvaEsquerda(duracao - 200);
    delay(200);
    logTrab("mover recuo esquerda");
    // macroCurvaDireita(duracao);
}

void macroCurvaDireita(int tempoMilisegundos)
{
    int duracao = _TEMPO_RECUO_;
    if (tempoMilisegundos > 0)
    {
        duracao = tempoMilisegundos;
    }
    setServoDirecao(SERVO_DIRECAO_MAX);
    moverFrente(_VELOCIDADE_CURVA_);
    delay(duracao);
    deslMotor();
    setServoDirecao(SERVO_CENTRO - 5);
    // servoDirecaoCentro();
}

void macroCurvaEsquerda(int tempoMilisegundos)
{
    int duracao = _TEMPO_RECUO_;
    if (tempoMilisegundos > 0)
    {
        duracao = tempoMilisegundos;
    }
    setServoDirecao(SERVO_DIRECAO_MIN);
    moverFrente(_VELOCIDADE_CURVA_);
    delay(duracao);
    deslMotor();
    setServoDirecao(SERVO_CENTRO + 5);
    // servoDirecaoCentro();
}

void curva(float distancia)
{
    if (distanciaDireita < distanciaEsquerda)
    {
        macroCurvaEsquerda(_TEMPO_CURVA_);
        // direcaoAnterior = 1;
    }
    else
    {
        macroCurvaDireita(_TEMPO_CURVA_);
        // direcaoAnterior = 0;
    }
    logTrab("mover curva direcao ->" + direcaoAnterior, (int)distancia);
}
void curvarRecuo(float distancia)
{
    if (distanciaDireita < distanciaEsquerda)
    {
        macroRecuoCurvaEsquerda(_TEMPO_CURVA_);
        // direcaoAnterior = 1;
    }
    else
    {
        macroRecuoCurvaDireita(_TEMPO_CURVA_);
        // direcaoAnterior = 0;
    }
    logTrab("mover recuo direcao ->" + direcaoAnterior, (int)distancia);
}
