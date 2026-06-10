#include "Estrategia.h"
#include "SensoresToF.h"
#include "SensorLinea.h"
#include "Motores.h"
#include "Comunicacion.h"
#include "ControlPID.h"
#include "Config.h"

static EstadoRobot estadoActual  = ESPERANDO_EN_ZONA;
static EstadoRobot estadoPrevio  = ESPERANDO_EN_ZONA;

// Estado no-bloqueante para evasión de línea
static unsigned long tiempoInicioEvasion = 0;
static bool          evasionActiva       = false;
static uint8_t       faseEvasion         = 0;
static int8_t        ladoLineaDetectado  = -1;

// Estado no-bloqueante para evasión de rival (duración mínima)
static unsigned long tiempoInicioEvasionRival = 0;
static bool          evasionRivalActiva       = false;

void inicializarEstrategia() {
    estadoActual             = ESPERANDO_EN_ZONA;
    estadoPrevio             = ESPERANDO_EN_ZONA;
    evasionActiva            = false;
    faseEvasion              = 0;
    tiempoInicioEvasion      = 0;
    ladoLineaDetectado       = -1;
    evasionRivalActiva       = false;
    tiempoInicioEvasionRival = 0;
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

    // Si la evasión de rival tiene duración mínima activa, no re-evaluar.
    if (evasionRivalActiva &&
        (millis() - tiempoInicioEvasionRival < TIEMPO_MIN_EVASION_RIVAL_MS)) {
        return;
    }
    evasionRivalActiva = false;

    // Prioridad 1: línea blanca
    if (detectarLineaBlanca()) {
        ladoLineaDetectado = obtenerLadoLinea();
        estadoActual = EVADIENDO_LINEA;
        return;
    }

    // Prioridad 2: oponente detectado
    // EXCEPCIÓN: si la cámara ve el balón cerca Y el ToF frontal detecta algo,
    // es probable que sea el balón — NO evadir.
    bool rivalFrente = detectarOponenteFrente();
    bool rivalIzq    = detectarOponenteIzquierda();
    bool rivalDer    = detectarOponenteDerecha();

    if (rivalFrente && datosCamara.balonDetectado &&
        datosCamara.distanciaEstimada < UMBRAL_DESPEJE + 20.0f) {
        // ToF frontal detecta algo pero la cámara ve el balón cerca → es el balón
        rivalFrente = false;
    }

    if (rivalFrente || rivalIzq || rivalDer) {
        if (estadoActual != EVADIENDO_RIVAL) {
            evasionRivalActiva       = true;
            tiempoInicioEvasionRival = millis();
        }
        estadoActual = EVADIENDO_RIVAL;
        return;
    }

    // Prioridad 3: balón — solo datos de cámara, sin odometría.
    // Histéresis: entra a DESPEJANDO en < (UMBRAL - HISTERESIS),
    //             sale de DESPEJANDO en > (UMBRAL + HISTERESIS).
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
        estadoActual = ESPERANDO_EN_ZONA;
    }
}

void ejecutarJugadaActual() {
    // Reset PID al entrar a INTERCEPTANDO desde otro estado
    if (estadoActual == INTERCEPTANDO && estadoPrevio != INTERCEPTANDO) {
        inicializarPID();
    }
    estadoPrevio = estadoActual;

    switch (estadoActual) {

    case EVADIENDO_LINEA: {
        unsigned long ahora = millis();
        if (!evasionActiva) {
            evasionActiva       = true;
            faseEvasion         = 0;
            tiempoInicioEvasion = ahora;
        }
        if (faseEvasion == 0) {
            // Fase 0: retroceder
            moverMotores(-255, -255);
            if (ahora - tiempoInicioEvasion >= 300) {
                faseEvasion         = 1;
                tiempoInicioEvasion = ahora;
            }
        } else {
            // Fase 1: girar HACIA EL LADO OPUESTO de donde se detectó la línea.
            // Q0 (VP) = sensor izquierdo → girar a la derecha (200, -200)
            // Q1 (VN) = sensor derecho  → girar a la izquierda (-200, 200)
            // Ambos o desconocido → girar a la derecha (default)
            if (ladoLineaDetectado == 1) {
                moverMotores(-200, 200);   // girar izquierda
            } else {
                moverMotores(200, -200);   // girar derecha (default / Q0 / ambos)
            }
            if (ahora - tiempoInicioEvasion >= 200) {
                evasionActiva = false;
                faseEvasion   = 0;
                estadoActual  = ESPERANDO_EN_ZONA;
            }
        }
        break;
    }

    case EVADIENDO_RIVAL: {
        bool opF = detectarOponenteFrente();
        bool opI = detectarOponenteIzquierda();
        bool opD = detectarOponenteDerecha();

        if ((opF && opI && opD) || (opI && opD)) {
            // Bloqueado por ambos lados (o por todos) → retroceder
            moverMotores(-150, -150);
        } else if (opF && !opI && !opD) {
            // Solo frente → retroceder y girar al lado con más espacio
            if (obtenerDistanciaIzquierda() >= obtenerDistanciaDerecha())
                moverMotores(-100, -200);  // retrocede curvando a la izquierda
            else
                moverMotores(-200, -100);  // retrocede curvando a la derecha
        } else if (opF && opI) {
            // Frente + izquierda → retroceder curvando a la derecha
            moverMotores(-200, -100);
        } else if (opF && opD) {
            // Frente + derecha → retroceder curvando a la izquierda
            moverMotores(-100, -200);
        } else if (opI && !opD) {
            // Solo izquierda → girar derecha (alejarse)
            moverMotores(150, -150);
        } else if (opD && !opI) {
            // Solo derecha → girar izquierda (alejarse)
            moverMotores(-150, 150);
        } else {
            // Caso residual → retroceder
            moverMotores(-150, -150);
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
        // Avanzar a máxima velocidad MIENTRAS corrige dirección.
        // Si la portería está alineada → recto a 255.
        // Si no, avanzar con diferencial para corregir sin detenerse.
        if (datosCamara.porteriaEnemigaAlineada) {
            moverMotores(255, 255);
        } else {
            float errorX = datosCamara.coordX - CAM_CENTRO_X;
            if (errorX > 20.0f) {
                // Balón a la derecha → curvar a la derecha (rápido/lento)
                moverMotores(255, 120);
            } else if (errorX < -20.0f) {
                // Balón a la izquierda → curvar a la izquierda
                moverMotores(120, 255);
            } else {
                // Centrado → recto a máxima
                moverMotores(255, 255);
            }
        }
        break;
    }

    case ESPERANDO_EN_ZONA:
        detenerRobot();
        break;
    }
}
