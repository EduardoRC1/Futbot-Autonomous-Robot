#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
//  Config.h — Definiciones centralizadas de pines, constantes y parámetros
//  Universidad de Matamoros — Futbot Autonomous Robot
//
//  TODOS los pines y constantes del hardware se definen AQUÍ.
//  Los demás módulos incluyen este archivo en lugar de definir pines localmente.
// ============================================================================

#include <Arduino.h>

// ---------------------------------------------------------------------------
// I2C Bus
// ---------------------------------------------------------------------------
static const int    PIN_I2C_SDA       = 16;
static const int    PIN_I2C_SCL       = 17;
static const uint32_t I2C_FRECUENCIA  = 100000;  // 100 kHz
static const uint16_t I2C_TIMEOUT_MS  = 100;

// ---------------------------------------------------------------------------
// VL53L0X Time-of-Flight — pines XSHUT
// ---------------------------------------------------------------------------
static const int PIN_TOF_FRONT_XSHUT = 5;
static const int PIN_TOF_LEFT_XSHUT  = 32;
static const int PIN_TOF_RIGHT_XSHUT = 33;  // Cambiado de 15 (strapping pin)

// Direcciones I2C asignadas a cada ToF (el default de fábrica es 0x29)
static const uint8_t TOF_ADDR_FRONT = 0x30;
static const uint8_t TOF_ADDR_LEFT  = 0x31;
static const uint8_t TOF_ADDR_RIGHT = 0x32;

// Umbrales para considerar que un objeto está "cerca" (mm)
static const uint16_t TOF_UMBRAL_OPONENTE_FRENTE_MM = 250;
static const uint16_t TOF_UMBRAL_OPONENTE_LATERAL_MM = 250;

// ---------------------------------------------------------------------------
// Motores (BTS7960) — canales PWM
// ---------------------------------------------------------------------------
static const int PIN_M1_R_PWM = 26;  // Motor izquierdo — avance
static const int PIN_M1_L_PWM = 25;  // Motor izquierdo — reversa
static const int PIN_M2_R_PWM = 14;  // Motor derecho   — avance
static const int PIN_M2_L_PWM = 27;  // Motor derecho   — reversa

static const int    PWM_FREQ       = 5000;  // Hz
static const int    PWM_RESOLUCION = 8;     // bits (0-255)
static const int    PWM_CH_M1_R    = 0;
static const int    PWM_CH_M1_L    = 1;
static const int    PWM_CH_M2_R    = 2;
static const int    PWM_CH_M2_L    = 3;

// ---------------------------------------------------------------------------
// Encoders (odometría) — según esquemático
// ---------------------------------------------------------------------------
static const int PIN_ENCODER_IZQ_A = 4;   // Motor izquierdo canal A (interrupción)
static const int PIN_ENCODER_IZQ_B = 18;  // Motor izquierdo canal B (dirección)
static const int PIN_ENCODER_DER_A = 19;  // Motor derecho canal A (interrupción)
static const int PIN_ENCODER_DER_B = 23;  // Motor derecho canal B (dirección)

// Parámetros de rueda — ajustar según hardware real
static const float DIAMETRO_LLANTA_CM = 6.5f;
static const float PULSOS_POR_VUELTA  = 330.0f;

// ---------------------------------------------------------------------------
// Cancha (cm) — límites de geocerca
// ---------------------------------------------------------------------------
static const float LIMITE_X_MAX =  214.0f;
static const float LIMITE_X_MIN =    5.0f;
static const float LIMITE_Y_MAX =   74.0f;
static const float LIMITE_Y_MIN =  -74.0f;

// Posición inicial del robot
static const float POS_INICIAL_X = 25.0f;
static const float POS_INICIAL_Y =  0.0f;

// ---------------------------------------------------------------------------
// PID — ganancias por defecto
// ---------------------------------------------------------------------------
static const float PID_KP_DEFAULT = 1.5f;
static const float PID_KI_DEFAULT = 0.0f;
static const float PID_KD_DEFAULT = 0.5f;
static const float PID_LIMITE_INTEGRAL = 500.0f;

// ---------------------------------------------------------------------------
// BNO055 IMU
// ---------------------------------------------------------------------------
static const uint8_t BNO055_DIRECCION_I2C = 0x28;  // Sin pin ADR = 0x28

// ---------------------------------------------------------------------------
// QTR-8A — sensor de línea (2 canales analógicos)
// ---------------------------------------------------------------------------
static const uint8_t PIN_QTR_1 = 36;  // VP
static const uint8_t PIN_QTR_2 = 39;  // VN
static const uint8_t QTR_NUM_CANALES = 2;
static const uint16_t QTR_UMBRAL_LINEA = 3500;  // Valor 0-4095, ajustar en cancha (subido de 500)

// ---------------------------------------------------------------------------
// Comunicación — timeout de la cámara (ms)
// ---------------------------------------------------------------------------
static const unsigned long TIMEOUT_CAMARA_MS = 500;

#endif // CONFIG_H