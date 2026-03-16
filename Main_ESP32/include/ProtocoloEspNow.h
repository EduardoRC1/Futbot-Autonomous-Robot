#ifndef PROTOCOLO_ESPNOW_H
#define PROTOCOLO_ESPNOW_H
#include <Arduino.h>

typedef struct MensajeVision {
    bool balonDetectado;      
    int16_t coordX;           
    int16_t coordY;           
    float distanciaEstimada;  
    bool porteriaEnemigaAlineada; 
} MensajeVision;

// Se tuvo que cambiar la direccion MAC de su direccion raw a hexadecimal
// Se tiene que usar la misma direccion en la CAM tambien
const uint8_t direccionMacCerebro[] = {0x00, 0x70, 0x07, 0x1C, 0x0F, 0xB0};
#endif
