// Estrategia.cpp, aqui se implementara la logica
// Que decide el robot hacer en cada momento, dependiendo de lo que detecten los sensores y la camara
#include "Estrategia.h"
#include "Sensores.h"
#include "Motores.h"
#include "Comunicacion.h"
#include "Odometria.h"
#include "ControlPID.h"

EstadoRobot estadoActual = ESPERANDO_EN_ZONA;

// Limites Virtuales de la cancha (219X158CM)
const float LIMITE_X_MAX = 214.0;
const float LIMITE_X_MIN = 5.0;
const float LIMITE_Y_MAX = 74.0;
const float LIMITE_Y_MIN = -74.0;

void inicializarEstrategia() {
    estadoActual = ESPERANDO_EN_ZONA;
}

void evaluarEntorno() {
    // Estas funciones se pueden llamar cualquier orden para evaluar el entorno
    float xRobot = obtenerCoordenadaX();
    float yRobot = obtenerCoordenadaY();

    if (detectarLineaBlanca() ||
        xRobot > LIMITE_X_MAX || xRobot < LIMITE_X_MIN ||
        yRobot > LIMITE_Y_MAX || yRobot < LIMITE_Y_MIN) {
        estadoActual = EVADIENDO_LINEA;
        return;
}
    if (detectarOponenteFrente()) {
        estadoActual = EVADIENDO_RIVAL;
        return;
    }

    float xBalonAproximado = xRobot + datosCamara.distanciaEstimada;

    if (datosCamara.balonDetectado) {
        if (xBalonAproximado > 109.5){
            estadoActual = REGRESANDO_A_BASE;
        } else if (xBalonAproximado <= 109.5 && datosCamara.distanciaEstimada > 15.0) {
            estadoActual = INTERCEPTANDO;
        } else {
            estadoActual = DESPEJANDO;
        } 
    } else {
            estadoActual = ESPERANDO_EN_ZONA;
        }
    }
// Esta funcion se llamara en el loop principal para ejecutar la jugada correspondiente al estado actual del robot
void ejecutarJugadaActual() {
    switch (estadoActual) {
        case EVADIENDO_LINEA: 
            moverRobot(-200, -200); // Reversa rapida
            delay (400); // Segun es suficiente tiempo para alejarse de la linea, se puede ajustar segun pruebas
            break;
        
        case EVADIENDO_RIVAL:
            pivotearIzquierda(150); // Esquivar choque
            break;

        case INTERCEPTANDO:
        // Aqui ControlPID se encargara de ajustar las velocidades para ir por el balon
        // Alguien tendra que escribir la logica para convertir los datos de la camara en un error de posicion
        // Ej. float errorX = datosCamara.coordX; // Si el balon esta a la derecha, el error es positivo, si esta a la izquierda, el error es negativo
            moverRobot(200, 200);
            break;

        case DESPEJANDO:
        {
            float anguloActual = leerRumboBrujula();
            if (anguloActual > 15.0 && anguloActual < 180.0) {
                pivotearIzquierda(120);
            } else if (anguloActual >= 180.0 && anguloActual < 345.0) {
                pivotearDerecha(120);
            } else {
                moverRobot(255, 255); 
            }
        }
        break;

        case REGRESANDO_A_BASE:
        // Velocidad moderada para volver a la porteria
        // Se usara Odometria para asegurar nos de volver a la porteria
        // Alguien tendra que escribir la logica para convertir las coordenadas actuales del robot en un error de posicion respecto a la porteria
            moverRobot(-150, -150);
            break;

        case ESPERANDO_EN_ZONA:
        // Mantener posicion, luego se puede agregar logica para patrullar de esquina a esquina dentro de nuestra zona
            detenerRobot();
            break;
    }
}
