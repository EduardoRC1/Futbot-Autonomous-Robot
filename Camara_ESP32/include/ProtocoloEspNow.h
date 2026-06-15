#ifndef PROTOCOLO_ESPNOW_H
#define PROTOCOLO_ESPNOW_H
#include <Arduino.h>

// =========================================================================
// 1. EL INTERRUPTOR DE ROBOTS (Cambia esto antes de subir el codigo)
// =========================================================================
//#define ROBOT_A // <- Quita las barras (//) para programar la Camara del Robot A
//#define ROBOT_B   // <- Deja esto asi para programar la Camara del Robot B
//#define ROBOT_HOTFIX  // Cerebro B + Cámara A (ESP32-A murió)
#define ROBOT_HOTFIX2 // Cerebro A (nueva) + Cámara B

// =========================================================================
// 2. LA ESTRUCTURA DE DATOS (Universal para ambos robots)
// =========================================================================
typedef struct MensajeVision {
    bool balonDetectado;
    int16_t coordX;
    int16_t coordY;
    float distanciaEstimada;
    bool porteriaEnemigaAlineada;
} MensajeVision;

// =========================================================================
// 3. CONFIGURACION DE DIRECCIONES MAC (Cruce de senales)
// =========================================================================

#ifdef ROBOT_A
  // Direcciones para el Robot A
  const uint8_t direccionMacCerebro[] = {0xB0, 0xCB, 0xD8, 0x0F, 0x86, 0xE8};
  const uint8_t direccionMacCamara[]  = {0x88, 0x57, 0x21, 0xC2, 0x0B, 0x68};

#elif defined(ROBOT_B)
  // Direcciones para el Robot B
  const uint8_t direccionMacCerebro[] = {0xB0, 0xCB, 0xD8, 0x0F, 0x5E, 0xF8};
  const uint8_t direccionMacCamara[]  = {0x8C, 0x94, 0xDF, 0x72, 0xA4, 0xE0};

#elif defined(ROBOT_HOTFIX)
  // Cerebro B (hardware) + Cámara A (hardware)
  const uint8_t direccionMacCerebro[] = {0xB0, 0xCB, 0xD8, 0x0F, 0x5E, 0xF8};  // MAC real del Cerebro B
  const uint8_t direccionMacCamara[]  = {0x88, 0x57, 0x21, 0xC2, 0x0B, 0x68};  // MAC real de Cámara A

#elif defined(ROBOT_HOTFIX2)
  // Cerebro A (hardware nuevo) + Cámara B (hardware)
  const uint8_t direccionMacCerebro[] = {0x68, 0x09, 0x47, 0x47, 0x27, 0xA0};  // MAC real del Cerebro A (nuevo)
  const uint8_t direccionMacCamara[]  = {0x8C, 0x94, 0xDF, 0x72, 0xA4, 0xE0};  // MAC real de Cámara B

#else
  #error "Definir ROBOT_A, ROBOT_B, ROBOT_HOTFIX o ROBOT_HOTFIX2 en ProtocoloEspNow.h"
#endif

#endif
