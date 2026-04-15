// Comunicacion.h
#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <Arduino.h>
#include "ProtocoloEspNow.h"

// Variable global para almacenar los datos recibidos de la camara 
extern MensajeVision datosCamara;

void inicializarRadio();
bool hayDatosNuevos();
void limpiarBanderaDatos();
void revisarConexionSegura();

#endif // COMUNICACION_H

