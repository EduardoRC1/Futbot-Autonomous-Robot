#include "Estrategia.h"
#include "Sensores.h"
#include "Motores.h"
#include "Comunicacion.h"
#include "Odometria.h"
#include "ControlPID.h"

EstadoRobot estadoActual = ESPERANDO_EN_ZONA;

const float LIMITE_X_MAX = 214.0; const float LIMITE_X_MIN = 5.0;
const float LIMITE_Y_MAX = 74.0; const float LIMITE_Y_MIN = -74.0;

void inicializarEstrategia() { estadoActual = ESPERANDO_EN_ZONA; }

void evaluarEntorno() {
    float xRobot = obtenerCoordenadaX(); float yRobot = obtenerCoordenadaY();

    if (detectarLineaBlanca() || xRobot > LIMITE_X_MAX || xRobot < LIMITE_X_MIN || yRobot > LIMITE_Y_MAX || yRobot < LIMITE_Y_MIN) {
        estadoActual = EVADIENDO_LINEA; return;
    }
    if (detectarOponenteFrente()) { estadoActual = EVADIENDO_RIVAL; return; }

    float xBalonAproximado = xRobot + datosCamara.distanciaEstimada;

    if (datosCamara.balonDetectado) {
        if (xBalonAproximado > 109.5){ estadoActual = REGRESANDO_A_BASE; } 
        else if (xBalonAproximado <= 109.5 && datosCamara.distanciaEstimada > 15.0) { estadoActual = INTERCEPTANDO; } 
        else { estadoActual = DESPEJANDO; } 
    } else {
        estadoActual = ESPERANDO_EN_ZONA;
    }
}

void ejecutarJugadaActual() {
    switch (estadoActual) {
        case EVADIENDO_LINEA: 
            moverRobot(-255, -255); // Reversa rapida
            delay(300); // 300ms es suficiente sin trabar el ESP32
            break;
        
        case EVADIENDO_RIVAL: pivotearIzquierda(150); break;

        case INTERCEPTANDO: {
            // Asumiendo resolucion de camara 320x240 (Centro en X = 160)
            float errorX = datosCamara.coordX - 160; 
            int ajusteVelocidad = calcularVelocidadPID(errorX);
            
            // Si la pelota esta a la derecha (error positivo), el motor izq acelera y el der frena
            int velIzq = 200 + ajusteVelocidad;
            int velDer = 200 - ajusteVelocidad;
            moverRobot(velIzq, velDer);
            break;
        }

        case DESPEJANDO: {
            float anguloActual = leerRumboBrujula();
            if (anguloActual > 15.0 && anguloActual < 180.0) { pivotearIzquierda(150); } 
            else if (anguloActual >= 180.0 && anguloActual < 345.0) { pivotearDerecha(150); } 
            else { moverRobot(255, 255); }
            break;
        }

        case REGRESANDO_A_BASE: moverRobot(-150, -150); break;
        case ESPERANDO_EN_ZONA: detenerRobot(); break;
    }
}