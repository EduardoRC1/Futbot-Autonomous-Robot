#include "Estrategia.h"
#include "SensoresToF.h"
#include "Motores.h"
#include "Comunicacion.h"
#include "ControlPID.h"
#include "Config.h"

static EstadoRobot estadoActual  = PATRULLANDO;
static EstadoRobot estadoPrevio  = PATRULLANDO;

void inicializarEstrategia() {
    estadoActual  = PATRULLANDO;
    estadoPrevio  = PATRULLANDO;
}

EstadoRobot obtenerEstadoActual() { return estadoActual; }

const char* nombreEstado(EstadoRobot estado) {
    switch (estado) {
        case PATRULLANDO:        return "PATRULLANDO";
        case INTERCEPTANDO:      return "INTERCEPTANDO";
        case DESPEJANDO:         return "DESPEJANDO";
        case ATACANDO_RIVAL:     return "ATACANDO";
        default:                 return "???";
    }
}

void evaluarEntorno() {
    // Prioridad 1: ToF detecta algo -> ATACAR (girar hacia ello y embestir)
    bool rivalFrente = detectarOponenteFrente();
    bool rivalIzq    = detectarOponenteIzquierda();
    bool rivalDer    = detectarOponenteDerecha();

    if (rivalFrente || rivalIzq || rivalDer) {
        estadoActual = ATACANDO_RIVAL;
        return;
    }

    // Prioridad 2: balón detectado por cámara
    if (datosCamara.balonDetectado) {
        float dist = datosCamara.distanciaEstimada;
        if (estadoActual == DESPEJANDO) {
            if (dist > UMBRAL_DESPEJE + HISTERESIS_DESPEJE)
                estadoActual = INTERCEPTANDO;
        } else {
            if (dist < UMBRAL_DESPEJE - HISTERESIS_DESPEJE)
                estadoActual = DESPEJANDO;
            else
                estadoActual = INTERCEPTANDO;
        }
    } else {
        estadoActual = PATRULLANDO;
    }
}

void ejecutarJugadaActual() {
    if (estadoActual == INTERCEPTANDO && estadoPrevio != INTERCEPTANDO) {
        inicializarPID();
    }
    estadoPrevio = estadoActual;

    switch (estadoActual) {

    case ATACANDO_RIVAL: {
        bool opF = detectarOponenteFrente();
        bool opI = detectarOponenteIzquierda();
        bool opD = detectarOponenteDerecha();

        if (opF) {
            // Rival al frente -> embestir recto a maxima
            moverMotores(255, 255);
        } else if (opI && !opD) {
            // Solo izquierda -> pivotear a la izquierda y embestir
            moverMotores(-255, 255);
        } else if (opD && !opI) {
            // Solo derecha -> pivotear a la derecha y embestir
            moverMotores(255, -255);
        } else if (opI && opD) {
            // Ambos lados -> embestir recto
            moverMotores(255, 255);
        } else {
            // Caso residual -> avanzar recto
            moverMotores(255, 255);
        }
        break;
    }

    case INTERCEPTANDO: {
        float errorX = datosCamara.coordX - CAM_CENTRO_X;
        int ajuste   = calcularVelocidadPID(errorX);
        moverMotores(constrain(255 + ajuste, -255, 255),
                     constrain(255 - ajuste, -255, 255));
        break;
    }

    case DESPEJANDO: {
        if (datosCamara.porteriaEnemigaAlineada) {
            moverMotores(255, 255);
        } else {
            float errorX = datosCamara.coordX - CAM_CENTRO_X;
            if (errorX > 20.0f) {
                moverMotores(255, 150);
            } else if (errorX < -20.0f) {
                moverMotores(150, 255);
            } else {
                moverMotores(255, 255);
            }
        }
        break;
    }

    case PATRULLANDO: {
        bool curvaDerecha = (millis() / PATRULLA_SEMIPERIODO_MS) % 2 == 0;
        if (curvaDerecha)
            moverMotores(PATRULLA_VEL_RAPIDA, PATRULLA_VEL_LENTA);
        else
            moverMotores(PATRULLA_VEL_LENTA, PATRULLA_VEL_RAPIDA);
        break;
    }
    }
}
