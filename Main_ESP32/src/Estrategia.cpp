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

    // Prioridad 3: lógica de balón — se decide SOLO con datos de la cámara.
    // No se usa odometría/brújula porque los encoders son poco confiables y
    // hacían que el robot creyera que el balón estaba pasando media cancha.
    if (datosCamara.balonDetectado) {
        if (datosCamara.distanciaEstimada > 15.0f) {
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

    case EVADIENDO_RIVAL: {
        evasionActiva = false;
        bool opI = detectarOponenteIzquierda();
        bool opD = detectarOponenteDerecha();

        if (opI && opD) {
            // Ambos lados bloqueados → retroceder para no quedar atorado
            moverMotores(-150, -150);
        } else if (opI && !opD) {
            // Oponente a la izquierda → girar a la derecha (lado libre)
            girarSuaveDerecha(150);
        } else if (opD && !opI) {
            // Oponente a la derecha → girar a la izquierda (lado libre)
            girarSuaveIzquierda(150);
        } else {
            // Oponente al frente → girar hacia el lado con MÁS espacio
            if (obtenerDistanciaIzquierda() >= obtenerDistanciaDerecha())
                girarSuaveIzquierda(150);
            else
                girarSuaveDerecha(150);
        }
        break;
    }

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
