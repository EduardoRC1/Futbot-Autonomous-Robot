#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

// ============================================================================
//  Estrategia.h — Máquina de estados para el robot defensa
// ============================================================================

#include <Arduino.h>

enum EstadoRobot {
    ESPERANDO_EN_ZONA,
    INTERCEPTANDO,
    DESPEJANDO,
    REGRESANDO_A_BASE,
    EVADIENDO_LINEA,
    EVADIENDO_RIVAL
};

void inicializarEstrategia();
void evaluarEntorno();
void ejecutarJugadaActual();

#endif // ESTRATEGIA_H
