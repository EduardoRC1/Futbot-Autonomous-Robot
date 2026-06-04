#ifndef COMUNICACION_H
#define COMUNICACION_H

// ============================================================================
//  Comunicacion.h — Radio ESP-NOW (recepción de datos de cámara)
// ============================================================================

#include <Arduino.h>
#include "ProtocoloEspNow.h"

// Últimos datos recibidos de la cámara.
// Llamar obtenerDatosCamara() antes de leer para garantizar copia segura.
extern MensajeVision datosCamara;

void inicializarRadio();
void obtenerDatosCamara();
bool hayDatosNuevos();
void limpiarBanderaDatos();
void revisarConexionSegura();

#endif // COMUNICACION_H
