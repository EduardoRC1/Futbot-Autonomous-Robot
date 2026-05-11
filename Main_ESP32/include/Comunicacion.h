// Comunicacion.h
// Universidad de Matamoros — Futbot Autonomous Robot
// Revisado: obtenerDatosCamara() agregada — debe llamarse antes de leer
//           datosCamara para garantizar una copia segura entre cores.

#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <Arduino.h>
#include "ProtocoloEspNow.h"

// ---------------------------------------------------------------------------
// datosCamara — variable pública con los últimos datos de la cámara.
// IMPORTANTE: Siempre llamar obtenerDatosCamara() antes de leerla en loop().
// ---------------------------------------------------------------------------
extern MensajeVision datosCamara;

void inicializarRadio();
void obtenerDatosCamara();   // Copia segura del buffer interno → datosCamara
bool hayDatosNuevos();
void limpiarBanderaDatos();
void revisarConexionSegura();

#endif // COMUNICACION_H