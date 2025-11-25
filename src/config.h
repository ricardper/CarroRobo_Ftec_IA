#ifndef _CONFIG_TRAB_H
#define _CONFIG_TRAB_H

// ======================= PINOS ===================================
#define SERVO_DIRECAO_PIN 14
#define SERVO_MEDICAO_PIN 27

// Motor L293D
#define MOTOR_ENA_PIN 25
#define MOTOR_IN1_PIN 33
#define MOTOR_IN2_PIN 32
#define VELOCIDADE_PADRAO_MOTOR_DC 50

// Velocidade do PWM do motor
#define MOTOR_PWM_CHANNEL 0
#define MOTOR_PWM_FREQ 1000
#define MOTOR_PWM_RESOLUTION 8 // 0–255
#define INCREMENTO_MOTOR_DC 10

// ======================= SERVO DIREÇÃO ============================
// Limites ABSOLUTOS – garantidos por clamp no servo_trab.cpp
#define SERVO_DIRECAO_MIN 35
#define SERVO_DIRECAO_MAX 130
#define SERVO_CENTRO 75

#define SERVO_ADICAO_SUBTRACAO 5 // incremento dos botões

// ======================= SERVO MEDIÇÃO ============================
#define SERVO_MEDICAO_PASSO 18
#define RADAR_MIN_ANGULO 0
#define RADAR_MAX_ANGULO 180

// ======================= WEBSERVER ================================
#define PORT_HTTP 80

// ======================= SERIAL ==================================
#define SERIAL_VELOCIDADE 115200

// ======================= LOOP / TEMPOS ============================
// Intervalo do radar → moveMedicao()
#define TIMEOUT_NILLIS 35 // ms

#define HCSR04_TRIG_PIN 12
#define HCSR04_ECHO_PIN 13
#define HCSR04_DIST_MAX_CM 200
#define HCSR04_DIST_MIN_CM 10

#define VL53L0X_DIST_MAX_CM 200
#define VL53L0X_DIST_MIN_CM 3

#define I2C_SDA_PIN 5
#define I2C_SCL_PIN 4

//======================= Define o tipo do wifi=======================
#define WIFI_AP_TRAB 0 // 0 = Cliente (Station), 1 = AP (Access Point)
#define CLIENT_SSID_WIFI "CONTAB_ 2.4"
#define CLIENT_PASSWORD_WIFI "11851404"
#define AP_SSID_WIFI "ESP32_Carro_Robo"
#define AP_PASSWORD_WIFI "123456789"

#endif
