// Estrategia.cpp
// Universidad de Matamoros — Futbot Autonomous Robot
// Revisado: moverRobot() → moverMotores(), EVADIENDO_LINEA no-bloqueante,
//           estimación de posición del balón con ángulo de brújula,
//           double-switch simplificado a uno solo limpio.

#include "Estrategia.h"
#include "Sensores.h"
#include "Motores.h"
#include "Comunicacion.h"
#include "Odometria.h"
#include "ControlPID.h"
#include <math.h>

EstadoRobot estadoActual = ESPERANDO_EN_ZONA;

// Límites de la cancha en centímetros
// X: largo de la cancha (5cm borde propio → 214cm borde rival)
// Y: ancho de la cancha (-74cm banda izquierda → +74cm banda derecha)
const float LIMITE_X_MAX =  214.0;
const float LIMITE_X_MIN =    5.0;
const float LIMITE_Y_MAX =   74.0;
const float LIMITE_Y_MIN =  -74.0;

// Temporizador no-bloqueante para EVADIENDO_LINEA
static unsigned long tiempoInicioEvasion = 0;
static bool          evasionActiva       = false;
static uint8_t       faseEvasion         = 0; // 0 = reversa, 1 = giro

void inicializarEstrategia() {
    estadoActual     = ESPERANDO_EN_ZONA;
    evasionActiva    = false;
    faseEvasion      = 0;
    tiempoInicioEvasion = 0;
}

// ---------------------------------------------------------------------------
// evaluarEntorno()
// Decide el estado del robot basándose en sensores y datos de cámara.
// No se llama mientras una evasión de línea está en curso (ver ejecutarJugadaActual).
// ---------------------------------------------------------------------------
void evaluarEntorno() {
    float xRobot = obtenerCoordenadaX();
    float yRobot = obtenerCoordenadaY();

    // 1. Línea blanca o geocerca — máxima prioridad
    if (detectarLineaBlanca()       ||
        xRobot > LIMITE_X_MAX       ||
        xRobot < LIMITE_X_MIN       ||
        yRobot > LIMITE_Y_MAX       ||
        yRobot < LIMITE_Y_MIN) {
        estadoActual = EVADIENDO_LINEA;
        return;
    }

    // 2. Oponente frente
    if (detectarOponenteFrente()) {
        estadoActual = EVADIENDO_RIVAL;
        return;
    }

    // 3. Lógica de balón
    if (datosCamara.balonDetectado) {
        // FIX: proyectar la posición del balón usando el ángulo real del robot,
        //      no solo en el eje X. Si el robot giró, el balón puede estar
        //      desplazado en Y, no en X.
        float anguloRad      = leerRumboBrujula() * (PI / 180.0);
        float xBalonAproximado = xRobot + datosCamara.distanciaEstimada * cos(anguloRad);

        if (xBalonAproximado > 109.5) {
            // Balón en campo rival — volver a defender
            estadoActual = REGRESANDO_A_BASE;
        } else if (datosCamara.distanciaEstimada > 15.0) {
            // Balón en nuestro campo y lejos — interceptar
            estadoActual = INTERCEPTANDO;
        } else {
            // Balón en nuestro campo y cerca — despejar
            estadoActual = DESPEJANDO;
        }
    } else {
        estadoActual = ESPERANDO_EN_ZONA;
    }
}

// ---------------------------------------------------------------------------
// ejecutarJugadaActual()
// Ejecuta la acción del estado actual. Un solo switch limpio.
// EVADIENDO_LINEA usa millis() — no bloquea el ESP32.
// ---------------------------------------------------------------------------
void ejecutarJugadaActual() {
    switch (estadoActual) {

        // ---------------------------------------------------------------
        case EVADIENDO_LINEA: {
            unsigned long ahora = millis();

            // Arrancar la secuencia de evasión la primera vez que entramos
            if (!evasionActiva) {
                evasionActiva       = true;
                faseEvasion         = 0;
                tiempoInicioEvasion = ahora;
            }

            if (faseEvasion == 0) {
                // Fase 0: reversa rápida 300ms
                moverMotores(-255, -255);
                if (ahora - tiempoInicioEvasion >= 300) {
                    faseEvasion         = 1;
                    tiempoInicioEvasion = ahora;
                }
            } else {
                // Fase 1: giro 200ms para alejarse de la línea
                moverMotores(200, -200);
                if (ahora - tiempoInicioEvasion >= 200) {
                    evasionActiva = false;
                    faseEvasion   = 0;
                    estadoActual  = ESPERANDO_EN_ZONA;
                }
            }
            break;
        }

        // ---------------------------------------------------------------
        case EVADIENDO_RIVAL:
            evasionActiva = false;
            // TODO: leer láseres izq/der para elegir el lado con más espacio libre.
            pivotearIzquierda(150);
            break;

        // ---------------------------------------------------------------
        case INTERCEPTANDO: {
            evasionActiva = false;
            // PID sobre eje X cámara (resolución 320x240, centro = 160px)
            float errorX          = datosCamara.coordX - 160.0;
            int   ajusteVelocidad = calcularVelocidadPID(errorX);

            // Error positivo = pelota a la derecha → motor izq acelera, der frena
            int velIzq = constrain(200 + ajusteVelocidad, -255, 255);
            int velDer = constrain(200 - ajusteVelocidad, -255, 255);
            moverMotores(velIzq, velDer);
            break;
        }

        // ---------------------------------------------------------------
        case DESPEJANDO: {
            evasionActiva = false;
            // Alinearse hacia 0° (portería rival) antes de avanzar
            float anguloActual = leerRumboBrujula();
            if (anguloActual > 15.0 && anguloActual < 180.0) {
                pivotearIzquierda(150);
            } else if (anguloActual >= 180.0 && anguloActual < 345.0) {
                pivotearDerecha(150);
            } else {
                moverMotores(255, 255); // Apunta al frente — despejar a fondo
            }
            break;
        }

        // ---------------------------------------------------------------
        case REGRESANDO_A_BASE:
            evasionActiva = false;
            moverMotores(-150, -150);
            break;

        // ---------------------------------------------------------------
        case ESPERANDO_EN_ZONA:
            evasionActiva = false;
            detenerRobot();
            break;
    }
}