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

// MAC Address de la placa ESP32 (solo es un ejemplo ahorita, tendremos que cambiar la luego)
// Nos falta un cable usb a micro usb
const uint8_t direccionMacCerebro[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#endif
