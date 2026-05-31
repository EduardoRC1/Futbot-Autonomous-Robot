#ifndef PROTOCOLO_ESPNOW_H
#define PROTOCOLO_ESPNOW_H
#include <Arduino.h>

// =========================================================================
// 1. EL INTERRUPTOR DE ROBOTS (Cambia esto antes de subir el codigo)
// =========================================================================
//#define ROBOT_A // <- Quita las barras (//) para programar la Camara del Robot A
#define ROBOT_B   // <- Deja esto asi para programar la Camara del Robot B

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
  const uint8_t direccionMacCerebro[] = {0x00, 0x70, 0x07, 0x1C, 0x0F, 0xB0};
  const uint8_t direccionMacCamara[]  = {0x88, 0x57, 0x21, 0xC2, 0x0B, 0x68};

#elif defined(ROBOT_B)
  // Direcciones para el Robot B
  const uint8_t direccionMacCerebro[] = {0x00, 0x70, 0x07, 0x1C, 0xA0, 0x84};
  const uint8_t direccionMacCamara[]  = {0x8C, 0x94, 0xDF, 0x72, 0xA4, 0xE0};

#else
  #error "Definir ROBOT_A o ROBOT_B en ProtocoloEspNow.h linea 8"
#endif

#endif
