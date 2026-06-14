#ifndef SENSOR_LINEA_H
#define SENSOR_LINEA_H

// ============================================================================
//  SensorLinea.h — Sensor QTR-8A de línea blanca (2 canales analógicos)
// ============================================================================

#include <Arduino.h>

// Inicializa y calibra el QTR-8A (mide la base del campo al arrancar).
void inicializarSensorLinea();

// Devuelve true si se detecta la línea blanca del borde de la cancha.
bool detectarLineaBlanca();

// Valor crudo del último read del canal QTR (0-4095) para diagnóstico.
uint16_t obtenerValorQTR(uint8_t canal);

// Lado QTR que disparó la última detección: 0=Q0, 1=Q1, -1=ninguno/ambos.
int8_t obtenerLadoLinea();

#endif // SENSOR_LINEA_H
