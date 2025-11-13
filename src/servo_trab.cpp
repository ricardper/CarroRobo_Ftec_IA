#include "servo_trab.h"

Servo servoDirecao;
Servo servoMedicao;

bool sentidoMedicao = true;
int posicaoMedicao = 0;
bool medicaoLigado = false;

// extern volatile int servoUltimaMedicao = 81;

volatile int servoUltimoValor = 81;

int initServo()
{

    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    servoDirecao.attach(SERVO_DIRECAO_PIN);
    servoMedicao.attach(SERVO_MEDICAO_PIN);
    servoMedicao.write(posicaoMedicao);

    return 0;
}

void servoSetValor(Servo *servo, int graus)
{
    servo->write(graus);
}

int setServoDirecao(int graus)
{
    if (graus != servoUltimoValor)
    {

        if (graus >= SERVO_DIRECAO_MIN && graus <= SERVO_DIRECAO_MAX)
        {
            logTrab("-valor da direcao antes ", servoUltimoValor);

            servoDirecao.write(graus);
            servoUltimoValor = graus;
            enviaDadosClientes("Direcao", graus);

            logTrab("-valor da direcao atual ", servoUltimoValor);
                }
    }

    return 0;
}

/*
centraliza a direcao no centro

*/
void servoDirecaoCentro()
{
    servoDirecao.write(SERVO_CENTRO);
}

void addDirecao()
{

    setServoDirecao(servoUltimoValor + SERVO_ADICAO_SUBTRACAO);
    logTrab("addDirecao -> ", servoUltimoValor);
}
void subDirecao()
{

    setServoDirecao(servoUltimoValor - SERVO_ADICAO_SUBTRACAO);
    logTrab("subDirecao -> ", servoUltimoValor);
}

int getDirecaoGraus()
{
    return servoDirecao.read();
}

void addMedicao()
{
    int posicao = posicaoMedicao + SERVO_MEDICAO_PASSO;
    if (posicao < 180)
    {
        servoMedicao.write(posicao);
        posicaoMedicao = posicao;
        sentidoMedicao = true;
    }
    else
    {
        servoMedicao.write(180);
        posicaoMedicao = 180;
        sentidoMedicao = false;
    }
}

void subMedicao()
{
    int posicao = posicaoMedicao - SERVO_MEDICAO_PASSO;
    if (posicao > 0)
    {
        servoMedicao.write(posicao);
        posicaoMedicao = posicao;
        sentidoMedicao = false;
    }
    else
    {
        servoMedicao.write(0);
        posicaoMedicao = 0;
        sentidoMedicao = true;
    }
}

void moveMedicao()
{
    if (medicaoLigado)
    {
        if (sentidoMedicao)
        {
            addMedicao();
        }
        else
        {
            subMedicao();
        }
    }
}

void ligaDesligaMedicao(int valor)
{
    if (valor == 1)
    {
        medicaoLigado = true;
        logTrab("medicao ligada");
    }
    if (valor == 0)
    {
        medicaoLigado = false;
        logTrab("medicao desligadaligada");
    }
}
