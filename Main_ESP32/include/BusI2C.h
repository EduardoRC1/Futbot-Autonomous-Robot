#ifndef BUS_I2C_H
#define BUS_I2C_H

// ============================================================================
//  BusI2C.h — Gestión del bus I2C: inicialización, escaneo y recuperación
// ============================================================================

#include <Arduino.h>

// Inicializa el bus I2C en los pines y frecuencia definidos en Config.h.
// Devuelve true si se pudo iniciar correctamente.
bool inicializarBusI2C();

// Escanea el bus I2C e imprime las direcciones de los dispositivos encontrados.
// Devuelve la cantidad de dispositivos detectados.
int escanearBusI2C();

// Intenta recuperar el bus si SDA quedó en LOW (bus colgado).
// Genera pulsos de reloj en SCL para liberar el bus.
void recuperarBusI2C();

#endif // BUS_I2C_H
