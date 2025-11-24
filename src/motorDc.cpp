// motorDc.cpp — VERSÃO FINAL OTIMIZADA

#include "motorDc.h"

int ultimaVelocidade = 0; // -100 a 100

// ============ Inicialização do motor ==============
void initMotoDc()
{
    pinMode(MOTOR_IN1_PIN, OUTPUT);
    pinMode(MOTOR_IN2_PIN, OUTPUT);
    pinMode(MOTOR_ENA_PIN, OUTPUT);

    ledcSetup(MOTOR_PWM_CHANNEL, MOTOR_PWM_FREQ, MOTOR_PWM_RESOLUTION);
    ledcAttachPin(MOTOR_ENA_PIN, MOTOR_PWM_CHANNEL);

    moverMotor(0); // para tudo no início
    logTrab("Motor L293D inicializado");
}

// ============ Aplica velocidade no motor ===========
void moverMotor(int velocidade)
{
    // Clamping absoluto
    if (velocidade > 100)
        velocidade = 100;
    if (velocidade < -100)
        velocidade = -100;

    // Sem mudança → não loga
    if (velocidade == ultimaVelocidade)
        return;

    int pwm = abs(velocidade) * 2.55; // converte -100..100 → PWM 0..255

    if (velocidade > 0)
    {
        digitalWrite(MOTOR_IN1_PIN, HIGH);
        digitalWrite(MOTOR_IN2_PIN, LOW);
    }
    else if (velocidade < 0)
    {
        digitalWrite(MOTOR_IN1_PIN, LOW);
        digitalWrite(MOTOR_IN2_PIN, HIGH);
    }
    else
    {
        digitalWrite(MOTOR_IN1_PIN, LOW);
        digitalWrite(MOTOR_IN2_PIN, LOW);
    }

    ledcWrite(MOTOR_PWM_CHANNEL, pwm);

    // Atualiza slider na homepage
    enviaDadosClientes("Velocidade", velocidade);

    logTrab("Velocidade antes " + String(ultimaVelocidade) + " agora " + String(velocidade));

    ultimaVelocidade = velocidade;
}

// ============ Controles incrementais ==================
void addVelocidade()
{
    moverMotor(ultimaVelocidade + INCREMENTO_MOTOR_DC);
}

void subVelocidade()
{
    moverMotor(ultimaVelocidade - INCREMENTO_MOTOR_DC);
}

void deslMotor()
{
    moverMotor(0);
    logTrab("Motor parado");
}
