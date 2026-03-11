#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>

// Pines XSHUT para encender los láseres VL53L0X uno por uno 
// y asignarles diferentes direcciones I2C
extern const int TOF_FRONT_XSHUT;
extern const int TOF_LEFT_XSHUT;
extern const int TOF_RIGHT_XSHUT;

// Funciones de configuración inicial
void inicializarBusI2C();
void inicializarIMU_BNO055();
void inicializarToF_VL53L0X();
void inicializarLinea_QTR8A();

// Funciones de lectura que tomarán las decisiones en la cancha
float leerRumboBrujula();         // Devuelve el ángulo actual del robot (0 a 360 grados)
bool detectarOponenteFrente();    // Devuelve true si un oponente está a menos de 20cm
bool detectarLineaBlanca();       // Devuelve true si el sensor infrarrojo detecta el borde

#endif
