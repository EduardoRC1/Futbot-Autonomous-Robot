#include "Estrategia.h"
#include "SensoresToF.h"
#include "Motores.h"
#include "Comunicacion.h"
#include "ControlPID.h"
#include "Config.h"

static EstadoRobot estadoActual  = PATRULLANDO;
static EstadoRobot estadoPrevio  = PATRULLANDO;

// Duración mínima de ataque — evita oscilar entre estados
static unsigned long tiempoInicioAtaqueRival = 0;
static bool          ataqueRivalActivo       = false;

void inicializarEstrategia() {
    estadoActual             = PATRULLANDO;
    estadoPrevio             = PATRULLANDO;
    ataqueRivalActivo        = false;
    tiempoInicioAtaqueRival  = 0;
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
    // Si el ataque a rival tiene duración mínima activa, no re-evaluar.
    if (ataqueRivalActivo &&
        (millis() - tiempoInicioAtaqueRival < TIEMPO_MIN_ATAQUE_RIVAL_MS)) {
        return;
    }
    ataqueRivalActivo = false;

    // Prioridad 1: ToF detecta algo → ATACAR (girar hacia ello y embestir)
    // EXCEPCIÓN: si la cámara ve el balón cerca Y el ToF frontal detecta algo,
    // es probable que sea el balón — NO atacar.
    bool rivalFrente = detectarOponenteFrente();
    bool rivalIzq    = detectarOponenteIzquierda();
    bool rivalDer    = detectarOponenteDerecha();

    if (rivalFrente && datosCamara.balonDetectado &&
        datosCamara.distanciaEstimada < UMBRAL_DESPEJE + 20.0f) {
        rivalFrente = false;
    }

    if (rivalFrente || rivalIzq || rivalDer) {
        if (estadoActual != ATACANDO_RIVAL) {
            ataqueRivalActivo       = true;
            tiempoInicioAtaqueRival = millis();
        }
        estadoActual = ATACANDO_RIVAL;
        return;
    }

    // Prioridad 2: balón — solo datos de cámara, sin odometría.
    if (datosCamara.balonDetectado) {
        float dist = datosCamara.distanciaEstimada;
        if (estadoActual == DESPEJANDO) {
            if (dist > UMBRAL_DESPEJE + HISTERESIS_DESPEJE)
                estadoActual = INTERCEPTANDO;
        } else {
            if (dist < UMBRAL_DESPEJE - HISTERESIS_DESPEJE)
                estadoActual = DESPEJANDO;
            else if (dist >= UMBRAL_DESPEJE - HISTERESIS_DESPEJE)
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
            moverMotores(255, 255);
        } else if (opI && !opD) {
            moverMotores(-200, 200);
        } else if (opD && !opI) {
            moverMotores(200, -200);
        } else if (opI && opD) {
            moverMotores(255, 255);
        } else {
            moverMotores(200, 200);
        }
        break;
    }

    case INTERCEPTANDO: {
        float errorX = datosCamara.coordX - CAM_CENTRO_X;
        int ajuste   = calcularVelocidadPID(errorX);
        moverMotores(constrain(200 + ajuste, -255, 255),
                     constrain(200 - ajuste, -255, 255));
        break;
    }

    case DESPEJANDO: {
        if (datosCamara.porteriaEnemigaAlineada) {
            moverMotores(255, 255);
        } else {
            float errorX = datosCamara.coordX - CAM_CENTRO_X;
            if (errorX > 20.0f) {
                moverMotores(255, 120);
            } else if (errorX < -20.0f) {
                moverMotores(120, 255);
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
