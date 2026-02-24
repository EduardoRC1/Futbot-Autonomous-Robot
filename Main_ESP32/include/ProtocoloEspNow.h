#ifndef PROTOCOLO_ESPNOW_H
#define PROTOCOLO_ESPNOW_H

#include <Arduino.h>

// Data structure that the ESP32-CAM will send to the Main ESP32
// WARNING: This struct must be EXACTLY the same on both microcontrollers
typedef struct MensajeVision {
    bool balonDetectado;      // true if the camera detects the orange mass
    int16_t coordX;           // X position of the ball relative to center (e.g., -160 to 160)
    int16_t coordY;           // Y position of the ball
    float distanciaEstimada;  // Approximate calculation of how far away the ball is
} MensajeVision;

// MAC Address of the Main ESP32 (The Receiver)
// Note: Your team will need to change these numbers 
// to the actual MAC address of your specific ESP32 board.
const uint8_t direccionMacCerebro[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#endif // PROTOCOLO_ESPNOW_H