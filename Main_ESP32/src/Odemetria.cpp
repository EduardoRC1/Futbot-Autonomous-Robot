#include "Odometria.h"
#include "Sensores.h"

const int ENCODER_IZQ_A = 2; // Hay que revisar si este pin es correcto para el encoder del motor izquierdo
const int ENCODER_DER_A = 4; // Igaul hay que revisar si este pin es correcto para el encoder del motor derecho

volatile long pulsosIzquierdos = 0; // volatile porque se modificara dentro de una interrupcion
volatile long pulsosDerechos = 0;

float posicionX =0.0;
float posicionY =0.0;

void contarPulsoIzquierdo() {
    pulsosIzquierdos++;
}
void contarPulsoDerecho() {
    pulsosDerechos++;
}

void inicializarOdemetria() {
    pinMode(ENCODER_IZQ_A, INPUT_PULLUP); // Configurar el pin del encoder izquierdo como entrada con resistencia pull-up
    pinMode(ENCODER_DER_A, INPUT_PULLUP); // Configurar el pin del encoder derecho como entrada con resistencia pull-up

    attachInterrupt(digitalPinToInterrupt(ENCODER_IZQ_A), contarPulsoIzquierdo, RISING); // Configurar interrupcion para el encoder izquierdo
    attachInterrupt(digitalPinToInterrupt(ENCODER_DER_A), contarPulsoDerecho, RISING); // Configurar interrupcion para el encoder derecho
}

void actualizarPosicion() {
    // Aqui se utilizara matematicas, trgonometria, para convertir los pulsos de las llantas en centimetros (X, Y)
    // El que sepa trig yo no
}

float obtenerCoordenadaX() {return posicionX; }
float obtenerCoordenadaY() {return posicionY; }
