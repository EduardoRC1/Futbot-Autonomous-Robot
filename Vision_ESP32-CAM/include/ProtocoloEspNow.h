#ifndef PROTOCOLO_ESPNOW_H
#define PROTOCOLO_ESPNOW_H

#include <Arduino.h>

typedef struct MensajeVision {
    bool balonDetectado;      
    int16_t coordX;           
    int16_t coordY;           
    float distanciaEstimada;  
    bool porteriaEnemigaAlineada; // TRUE si vemos la porteria azul/amarilla
} MensajeVision;

// MAC Address of the MAIN ESP32 (Replace 0xFF later!)
const uint8_t direccionMacCerebro[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#endif