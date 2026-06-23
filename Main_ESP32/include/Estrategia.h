#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

// ============================================================================
//  Estrategia.h — Máquina de estados para el robot
// ============================================================================

#include <Arduino.h>

enum EstadoRobot {
    PATRULLANDO,        // Sin balón: se mueve barriendo su zona (nunca se detiene)
    INTERCEPTANDO,
    DESPEJANDO,
    ATACANDO_RIVAL      // ToF detecta algo → girar hacia ello y embestir a 255
};

void inicializarEstrategia();
void evaluarEntorno();
void ejecutarJugadaActual();
EstadoRobot obtenerEstadoActual();
const char* nombreEstado(EstadoRobot estado);

#endif // ESTRATEGIA_H
