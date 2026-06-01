#ifndef PROTOCOLO_ESPNOW_H
#define PROTOCOLO_ESPNOW_H

// ============================================================================
//  ProtocoloEspNow.h — Estructura de datos y MACs para comunicación ESP-NOW
// ============================================================================

#include <Arduino.h>

// Selector de robot — descomentar la línea correspondiente antes de compilar
#define ROBOT_A
//#define ROBOT_B

typedef struct MensajeVision {
    bool  balonDetectado;
    int16_t coordX;
    int16_t coordY;
    float distanciaEstimada;
    bool  porteriaEnemigaAlineada;
} MensajeVision;

#ifdef ROBOT_A
  const uint8_t direccionMacCerebro[] = {0x00, 0x70, 0x07, 0x1C, 0x0F, 0xB0};
  const uint8_t direccionMacCamara[]  = {0x88, 0x57, 0x21, 0xC2, 0x0B, 0x68};
#elif defined(ROBOT_B)
  const uint8_t direccionMacCerebro[] = {0x00, 0x70, 0x07, 0x1C, 0xA0, 0x84};
  const uint8_t direccionMacCamara[]  = {0x8C, 0x94, 0xDF, 0x72, 0xA4, 0xE0};
#else
  #error "Definir ROBOT_A o ROBOT_B en ProtocoloEspNow.h linea 13"
#endif

#endif // PROTOCOLO_ESPNOW_H
