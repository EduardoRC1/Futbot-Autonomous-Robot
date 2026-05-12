#ifndef ODOMETRIA_H
#define ODOMETRIA_H

// ============================================================================
//  Odometria.h — Posición del robot por encoders
// ============================================================================

#include <Arduino.h>

void inicializarOdometria();
void actualizarPosicion();
float obtenerCoordenadaX();
float obtenerCoordenadaY();

#endif // ODOMETRIA_H
