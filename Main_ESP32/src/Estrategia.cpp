#include "Estrategia.h"
#include "SensoresToF.h"
#include "SensorLinea.h"
#include "Motores.h"
#include "Comunicacion.h"
#include "ControlPID.h"
#include "Config.h"

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
        case EVADIENDO_LINEA:    return "EVAD_LINEA";
        case EVADIENDO_RIVAL:    return "EVAD_RIVAL";
        default:                 return "???";
    }
}

void evaluarEntorno() {
    // Si la evasión de línea está en progreso, dejarla terminar sin
    // re-evaluar — la secuencia retroceder→girar debe completarse.
    if (evasionActiva) return;

    // Prioridad 1: línea blanca
    if (detectarLineaBlanca()) {
        estadoActual = EVADIENDO_LINEA;
        return;
    }

    // Prioridad 2: oponente detectado
    if (detectarOponenteFrente() || detectarOponenteIzquierda() || detectarOponenteDerecha()) {
        estadoActual = EVADIENDO_RIVAL;
        return;
    }

    // Prioridad 3: balón — solo datos de cámara, sin odometría.
    // UMBRAL_DESPEJE calibrado para la escala de distanciaEstimada (5000/sqrt(px)).
    if (datosCamara.balonDetectado) {
        if (datosCamara.distanciaEstimada > UMBRAL_DESPEJE) {
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
        bool opI = detectarOponenteIzquierda();
        bool opD = detectarOponenteDerecha();

        if (opI && opD) {
            moverMotores(-150, -150);
        } else if (opI && !opD) {
            girarSuaveDerecha(150);
        } else if (opD && !opI) {
            girarSuaveIzquierda(150);
        } else {
            if (obtenerDistanciaIzquierda() >= obtenerDistanciaDerecha())
                girarSuaveIzquierda(150);
            else
                girarSuaveDerecha(150);
        }
        break;
    }

    case INTERCEPTANDO: {
        float errorX = datosCamara.coordX - 160.0f;
        int ajuste   = calcularVelocidadPID(errorX);
        moverMotores(constrain(200 + ajuste, -255, 255),
                     constrain(200 - ajuste, -255, 255));
        break;
    }

    case DESPEJANDO: {
        // Usa coordX de la cámara para decidir hacia dónde despejar.
        // Si la portería está alineada → avanzar recto a máxima velocidad.
        // Si no, pivotear hacia el centro del frame para corregir ángulo.
        if (datosCamara.porteriaEnemigaAlineada) {
            moverMotores(255, 255);
        } else {
            float errorX = datosCamara.coordX - 160.0f;
            if (errorX > 20.0f)
                pivotearDerecha(150);
            else if (errorX < -20.0f)
                pivotearIzquierda(150);
            else
                moverMotores(255, 255);
        }
        break;
    }

    case ESPERANDO_EN_ZONA:
        detenerRobot();
        break;
    }
}
