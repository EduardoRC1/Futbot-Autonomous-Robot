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

// Umbrales de detección ToF (mm). Detectar rival y atacarlo.
static const uint16_t TOF_UMBRAL_OPONENTE_FRENTE_MM = 300;
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

// Corrección de cableado por robot — cambiar a true si un motor gira al revés.
// Eva (HOTFIX2): si "retroceder" va hacia adelante, pon ambos en true.
// Wall-E (HOTFIX): normalmente false.
static const bool INVERTIR_MOTOR_IZQ   = true;
static const bool INVERTIR_MOTOR_DER   = true;
// true si M1 (código: izquierdo) está conectado físicamente al motor DERECHO
static const bool INTERCAMBIAR_MOTORES = false;

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
// QTR-8A — DESACTIVADO (modo sumo, sin línea)
// ---------------------------------------------------------------------------
static const uint8_t PIN_QTR_1 = 36;
static const uint8_t PIN_QTR_2 = 39;
static const uint8_t QTR_NUM_CANALES = 2;
static const uint16_t QTR_MARGEN_LINEA = 700;

// ---------------------------------------------------------------------------
// Cámara — centro horizontal del frame (ancho / 2)
// La estrategia centra el balón usando errorX = coordX - CAM_CENTRO_X.
// DEBE coincidir con la resolución elegida en el firmware de la cámara:
//   QVGA (320 ancho) -> 160 ;  VGA (640 ancho) -> 320
// ---------------------------------------------------------------------------
static const float CAM_CENTRO_X = 320.0f;  // VGA por defecto (USAR_VGA activo)

// ---------------------------------------------------------------------------
// Estrategia — umbral de distancia para DESPEJANDO
// La cámara ahora estima la distancia por el TAMAÑO APARENTE del balón:
//   distanciaEstimada = CONST_DISTANCIA_BLOB / diámetro_px  (MENOR = más cerca).
// La escala es la MISMA en QVGA y VGA (la K se ajusta por resolución), así que
// estos umbrales sirven para ambas.
//
// Equivalencia aproximada distancia -> diámetro del balón en el frame:
//   dist ~360 -> diám ~11 px  (lejos, recién detectado)  -> INTERCEPTANDO
//   dist ~170 -> diám ~24 px  (media cancha)
//   dist ~110 -> diám ~36 px  (cerca)                     -> DESPEJANDO
//   dist  ~67 -> diám ~60 px  (muy cerca)
//
// PROBLEMA ANTERIOR: la "distancia" se calculaba con el conteo total de píxeles,
// que casi nunca bajaba del umbral, así que el estado se quedaba "trabado" en
// INTERCEPTANDO. Con el tamaño del blob la distancia es confiable y DESPEJANDO
// es alcanzable. AJUSTAR en cancha con el log ("dist=..." y "blob=...").
// ---------------------------------------------------------------------------
static const float UMBRAL_DESPEJE      = 140.0f;
static const float HISTERESIS_DESPEJE  =  30.0f; // Entra a DESPEJANDO en <110, sale en >170

// Duración mínima de ataque a rival (ms) — evita oscilar entre estados
static const unsigned long TIEMPO_MIN_ATAQUE_RIVAL_MS = 400;

// ---------------------------------------------------------------------------
// Comunicación — timeout de la cámara (ms)
// ---------------------------------------------------------------------------
static const unsigned long TIMEOUT_CAMARA_MS = 500;

// ---------------------------------------------------------------------------
// Patrullaje — el robot nunca se detiene; barre su zona buscando el balón.
// ---------------------------------------------------------------------------
static const int           PATRULLA_VEL_RAPIDA   = 170;   // rueda exterior
static const int           PATRULLA_VEL_LENTA    = 90;    // rueda interior
static const unsigned long PATRULLA_SEMIPERIODO_MS = 1200; // cambio de lado

#endif // CONFIG_H