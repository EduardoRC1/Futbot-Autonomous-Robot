// Estrategia.cpp, aqui se implementara la logica
// Que decide el robot hacer en cada momento, dependiendo de lo que detecten los sensores y la camara
#include "Estrategia.h"
#include "Sensores.h"
#include "Motores.h"
#include "Comunicacion.h"

EstadoRobot estadoActual = ESPERANDO_EN_ZONA;

void inicializarEstrategia() {
    estadoActual = ESPERANDO_EN_ZONA;
}

void evaluarEntorno() {

    if (detectarLineaBlanca()) {
        estadoActual = EVADIENDO_LINEA;
        return;
}
    if (detectarOponenteFrente()) {
        estadoActual = EVADIENDO_RIVAL;
        return;
    }

    if (datosCamara.balonDetectado) {
        if (datosCamara.distanciaEstimada < 40.0) {
            if (datosCamara.distanciaEstimada < 10.0) {
                estadoActual = DESPEJANDO;
            } else {
                estadoActual = INTERCEPTANDO;
            }
        } else {
            estadoActual = ESPERANDO_EN_ZONA;
        }
    } else {
        estadoActual = REGRESANDO_A_BASE;

    }
}

// Esta funcion se llamara en el loop principal para ejecutar la jugada correspondiente al estado actual del robot
void ejectuarJugadaActual() {
    switch (estadoActual) {
        case EVADIENDO_LINEA: 
            moverRobot(-200, -200); // Reversa rapida
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
        // Maxima potencia para despejar el balon lo mas lejos posible
            moverRobot(255, 255);
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
