#include "Estrategia.h"
#include "SensoresToF.h"
#include "SensorIMU.h"
#include "SensorLinea.h"
#include "Motores.h"
#include "Comunicacion.h"
#include "Odometria.h"
#include "ControlPID.h"
#include "Config.h"
#include <math.h>

static EstadoRobot estadoActual = ESPERANDO_EN_ZONA;

// Estado no-bloqueante para evasión de línea
static unsigned long tiempoInicioEvasion = 0;
static bool          evasionActiva       = false;
static uint8_t       faseEvasion         = 0;

void inicializarEstrategia() {
    estadoActual        = ESPERANDO_EN_ZONA;
    evasionActiva       = false;
    faseEvasion         = 0;
    tiempoInicioEvasion = 0;
}

EstadoRobot obtenerEstadoActual() { return estadoActual; }

const char* nombreEstado(EstadoRobot estado) {
    switch (estado) {
        case ESPERANDO_EN_ZONA:  return "ESPERANDO";
        case INTERCEPTANDO:      return "INTERCEPTANDO";
        case DESPEJANDO:         return "DESPEJANDO";
        case REGRESANDO_A_BASE:  return "REGRESANDO";
        case EVADIENDO_LINEA:    return "EVAD_LINEA";
        case EVADIENDO_RIVAL:    return "EVAD_RIVAL";
        default:                 return "???";
    }
}

void evaluarEntorno() {
    float xRobot = obtenerCoordenadaX();
    float yRobot = obtenerCoordenadaY();

    // Prioridad 1: línea blanca (solo QTR — geocerca deshabilitada
    // porque los encoders generan pulsos falsos por ruido de motores)
    if (detectarLineaBlanca()) {
        estadoActual = EVADIENDO_LINEA;
        return;
    }

    // Prioridad 2: oponente detectado (frente, izquierda o derecha)
    if (detectarOponenteFrente() || detectarOponenteIzquierda() || detectarOponenteDerecha()) {
        estadoActual = EVADIENDO_RIVAL;
        return;
    }

    // Prioridad 3: lógica de balón
    if (datosCamara.balonDetectado) {
        float anguloRad        = leerRumboBrujula() * (PI / 180.0f);
        float xBalonAprox      = xRobot + datosCamara.distanciaEstimada * cosf(anguloRad);

        if (xBalonAprox > 109.5f) {
            estadoActual = REGRESANDO_A_BASE;
        } else if (datosCamara.distanciaEstimada > 15.0f) {
            estadoActual = INTERCEPTANDO;
        } else {
            estadoActual = DESPEJANDO;
        }
    } else {
        estadoActual = ESPERANDO_EN_ZONA;
    }
}

void ejecutarJugadaActual() {
    switch (estadoActual) {

    case EVADIENDO_LINEA: {
        unsigned long ahora = millis();
        if (!evasionActiva) {
            evasionActiva       = true;
            faseEvasion         = 0;
            tiempoInicioEvasion = ahora;
        }
        if (faseEvasion == 0) {
            moverMotores(-255, -255);
            if (ahora - tiempoInicioEvasion >= 300) {
                faseEvasion         = 1;
                tiempoInicioEvasion = ahora;
            }
        } else {
            moverMotores(200, -200);
            if (ahora - tiempoInicioEvasion >= 200) {
                evasionActiva = false;
                faseEvasion   = 0;
                estadoActual  = ESPERANDO_EN_ZONA;
            }
        }
        break;
    }

    case EVADIENDO_RIVAL:
        evasionActiva = false;
        if (detectarOponenteIzquierda() && !detectarOponenteDerecha())
            pivotearDerecha(150);
        else if (detectarOponenteDerecha() && !detectarOponenteIzquierda())
            pivotearIzquierda(150);
        else if (detectarOponenteFrente())
            pivotearIzquierda(150);
        else
            pivotearIzquierda(150);
        break;

    case INTERCEPTANDO: {
        evasionActiva = false;
        float errorX = datosCamara.coordX - 160.0f;
        int ajuste   = calcularVelocidadPID(errorX);
        moverMotores(constrain(200 + ajuste, -255, 255),
                     constrain(200 - ajuste, -255, 255));
        break;
    }

    case DESPEJANDO: {
        evasionActiva = false;
        float angulo = leerRumboBrujula();
        if (angulo > 15.0f && angulo < 180.0f)
            pivotearIzquierda(150);
        else if (angulo >= 180.0f && angulo < 345.0f)
            pivotearDerecha(150);
        else
            moverMotores(255, 255);
        break;
    }

    case REGRESANDO_A_BASE:
        evasionActiva = false;
        moverMotores(-150, -150);
        break;

    case ESPERANDO_EN_ZONA:
        evasionActiva = false;
        detenerRobot();
        break;
    }
}
