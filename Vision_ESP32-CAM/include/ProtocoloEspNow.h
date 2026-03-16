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

// Se agrego la direccion MAC del cerebro

const uint8_t direccionMacCerebro[] = {0x00, 0x70, 0x07, 0x1C, 0x0F, 0xB0};

#endif
