#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

// ============================================================================
//  Estrategia.h — Máquina de estados para el robot defensa
// ============================================================================

#include <Arduino.h>

enum EstadoRobot {
    PATRULLANDO,        // Sin balón: se mueve barriendo su zona (nunca se detiene)
    INTERCEPTANDO,
    DESPEJANDO,
    EVADIENDO_LINEA,
    EVADIENDO_RIVAL
};

void inicializarEstrategia();
void evaluarEntorno();
void ejecutarJugadaActual();
EstadoRobot obtenerEstadoActual();
const char* nombreEstado(EstadoRobot estado);

#endif // ESTRATEGIA_H
