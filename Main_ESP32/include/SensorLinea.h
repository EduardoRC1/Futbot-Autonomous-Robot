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

// Devuelve el valor crudo del canal QTR (0-4095) para calibración.
uint16_t obtenerValorQTR(uint8_t canal);

#endif // SENSOR_LINEA_H
