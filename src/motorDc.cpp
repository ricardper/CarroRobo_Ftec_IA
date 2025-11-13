// motorDc.cpp

#include "motorDc.h"

L293D motor1(MOTOR_IN1_A, MOTOR_IN1_B, MOTOR_1_PWM);

volatile int velocidadeAtual;

int initMotoDc()
{
    motor1.begin(true, PWM_MOTOR_FREQUENCY, PWM_MOTOR_RESOLUTION);
    motor1.SetMotorSpeed(0);
    velocidadeAtual = 0;
    return 0;
}
void moverFrente(int valor)
{
    if (velocidadeAtual == valor)
    {
        return;
    }
    if (valor >= 0 && valor <= 100)
    {
        motor1.SetMotorSpeed(valor);
        velocidadeAtual = valor;
    }
}

void moverTraz(int valor)
{
    if (velocidadeAtual == valor)
    {
        return;
    }
    if (valor <= 0 && valor >= -100)
    {
        motor1.SetMotorSpeed(valor);
        velocidadeAtual = valor;
    }
}

void moverMotor(int valor)
{

    if (velocidadeAtual == valor)
    {
        return;
    }

    if (valor >= -100 && valor <= 100)
    {
        motor1.SetMotorSpeed(valor);
        velocidadeAtual = valor;
        enviaDadosClientes("Velocidade", valor);
        logTrab("Velocidade -> ", velocidadeAtual);
    }

    return;
}

void subVelocidade()
{

    moverMotor(velocidadeAtual - MOTOR_ADICAO_SUBTRACAO);
    logTrab("subVelocidade -> ", velocidadeAtual);
}

void addVelocidade()
{

    moverMotor(velocidadeAtual + MOTOR_ADICAO_SUBTRACAO);
    logTrab("subVelocidade -> ", velocidadeAtual);
}

void deslMotor()
{
    moverMotor(0);
}
