#ifndef PROTOCOLO_ESPNOW_H
#define PROTOCOLO_ESPNOW_H
#include <Arduino.h>

// =========================================================================
// 1. EL INTERRUPTOR DE ROBOTS (Cambia esto antes de subir el código)
// =========================================================================
#define ROBOT_A  // <- Deja esto así para programar el Cerebro del Robot A
// #define ROBOT_B // <- Quita las barras (//) para programar el Cerebro del Robot B

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
// 3. CONFIGURACIÓN DE DIRECCIONES MAC (Cruce de señales)
// =========================================================================

#ifdef ROBOT_A
  // Direcciones para el Robot A
  const uint8_t direccionMacCerebro[] = {0x00, 0x70, 0x07, 0x1C, 0x0F, 0xB0};
  const uint8_t direccionMacCamara[]  = {0x88, 0x57, 0x21, 0xC2, 0x0B, 0x68};

#elif defined(ROBOT_B)
  // Direcciones para el Robot B
  // NOTA: Usa la MAC de la Cámara B que acabas de encontrar en el monitor
  const uint8_t direccionMacCerebro[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Falta descubrir Cerebro B
  const uint8_t direccionMacCamara[]  = {0x88, 0x57, 0x21, 0xC3, 0x44, 0x58};

#else
  #error "¡ALTO! Tienes que definir si vas a programar el ROBOT_A o el ROBOT_B en la línea 7"
#endif

#endif