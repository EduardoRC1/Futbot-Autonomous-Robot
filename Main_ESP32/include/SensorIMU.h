#ifndef SENSOR_IMU_H
#define SENSOR_IMU_H

// ============================================================================
//  SensorIMU.h — Sensor BNO055 (brújula / orientación)
//  NOTA: Módulo preparado para integración futura del BNO055.
// ============================================================================

#include <Arduino.h>

// Inicializa el BNO055 en el bus I2C.
// Devuelve true si se conectó correctamente.
bool inicializarIMU();

// Devuelve el rumbo actual del robot en grados (0–360).
float leerRumboBrujula();

#endif // SENSOR_IMU_H
