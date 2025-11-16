#ifndef _CONFIG_TRAB_H
#define _CONFIG_TRAB_H

// ======================= PINOS ===================================
#define SERVO_DIRECAO_PIN 14
#define SERVO_MEDICAO_PIN 27

// Motor L293D
#define MOTOR_ENA_PIN 25
#define MOTOR_IN1_PIN 33
#define MOTOR_IN2_PIN 32

// Velocidade do PWM do motor
#define MOTOR_PWM_CHANNEL 0
#define MOTOR_PWM_FREQ 1000
#define MOTOR_PWM_RESOLUTION 8 // 0–255

// ======================= SERVO DIREÇÃO ============================
// Limites ABSOLUTOS – garantidos por clamp no servo_trab.cpp
#define SERVO_DIRECAO_MIN 35
#define SERVO_DIRECAO_MAX 130
#define SERVO_CENTRO 85

#define SERVO_ADICAO_SUBTRACAO 5 // incremento dos botões

// ======================= SERVO MEDIÇÃO ============================
#define SERVO_MEDICAO_PASSO 2 // suavidade do radar

// ======================= WEBSERVER ================================
#define PORT_HTTP 80

// ======================= SERIAL ==================================
#define SERIAL_VELOCIDADE 115200

// ======================= LOOP / TEMPOS ============================
// Intervalo do radar → moveMedicao()
#define TIMEOUT_NILLIS 30 // ms

#define HCSR04_TRIG_PIN 26
#define HCSR04_ECHO_PIN 4
#define HCSR04_DIST_MAX_CM 200
#define HCSR04_DIST_MIN_CM 10

#define SSID_WIFI "CONTAB_ 2.4"
#define PASSWORD_WIFI "11851404"

#endif
