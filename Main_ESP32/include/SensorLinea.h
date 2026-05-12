#ifndef SENSOR_LINEA_H
#define SENSOR_LINEA_H

// ============================================================================
//  SensorLinea.h — Sensor QTR-8A de línea blanca
//  NOTA: Módulo preparado para integración futura del QTR-8A.
// ============================================================================

#include <Arduino.h>

// Inicializa el arreglo de sensores QTR-8A.
void inicializarSensorLinea();

// Devuelve true si se detecta la línea blanca del borde de la cancha.
bool detectarLineaBlanca();

#endif // SENSOR_LINEA_H
