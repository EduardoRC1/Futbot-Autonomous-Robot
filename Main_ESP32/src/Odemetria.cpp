#include "Odometria.h"
#include "Sensores.h"
#include <math.h>

//NOTA IMPORTANTEEEEEEEE
//Estos dos variables se tienen que ajustar segun las llantas y encoders que se usen, para convertir los pulsos en centimetros
const float DIAMETRO_LLANTA_CM = 6.5;
const float PULSOS_POR_VUELTA = 330.0;

const float DISTANCIA_POR_PULSO = (PI * DIAMETRO_LLANTA_CM) / PULSOS_POR_VUELTA; // Distancia que recorre el robot por cada pulso del encoder

const int ENCODER_IZQ_A = 2; // Hay que revisar si este pin es correcto para el encoder del motor izquierdo
const int ENCODER_DER_A = 4; // Igaul hay que revisar si este pin es correcto para el encoder del motor derecho

volatile long pulsosIzquierdos = 0; // volatile porque se modificara dentro de una interrupcion
volatile long pulsosDerechos = 0;

long pulsosIzquierdosAnteriores = 0;
long pulsosDerechosAnteriores = 0;

float posicionX = 25.0; // Posicion inicial del robot en X, se puede ajustar segun la posicion inicial real del robot en la cancha
float posicionY =0.0; // Posicion inicial del robot en Y, se puede ajustar segun la posicion inicial real del robot en la cancha

void IRAM_ATTR contarPulsoIzquierdo() {pulsosIzquierdos++;}
void IRAM_ATTR contarPulsoDerecho() {pulsosDerechos++;}

void inicializarOdemetria() {
    pinMode(ENCODER_IZQ_A, INPUT_PULLUP); // Configurar el pin del encoder izquierdo como entrada con resistencia pull-up
    pinMode(ENCODER_DER_A, INPUT_PULLUP); // Configurar el pin del encoder derecho como entrada con resistencia pull-up
    attachInterrupt(digitalPinToInterrupt(ENCODER_IZQ_A), contarPulsoIzquierdo, RISING); // Configurar interrupcion para el encoder izquierdo
    attachInterrupt(digitalPinToInterrupt(ENCODER_DER_A), contarPulsoDerecho, RISING); // Configurar interrupcion para el encoder derecho
}

// Esta funcion se llamara en el loop principal para actualizar la posicion del robot
// En la cancha de futbol, el eje X corre a lo largo de la cancha (de un arco al otro) y el eje Y corre a lo ancho de la cancha (de una banda a otra)
void actualizarPosicion() {
    long deltaIzquierda = pulsosIzquierdos - pulsosIzquierdosAnteriores;
    long deltaDerecha = pulsosDerechos - pulsosDerechosAnteriores;
// Actualizar los pulsos anteriores para la proxima iteracion
    pulsosIzquierdosAnteriores = pulsosIzquierdos;
    pulsosDerechosAnteriores = pulsosDerechos;
// Calcular la distancia recorrida por cada rueda
    float distanciaCentro = ((deltaIzquierda + deltaDerecha) / 2.0) * DISTANCIA_POR_PULSO;
// Calcular el nuevo angulo del robot usando la brujula
    float anguloGrados = leerRumboBrujula();
    float anguloRadianes = anguloGrados * (PI / 180.0);
// Actualizar la posicion del robot usando trigonometria
    posicionX += distanciaCentro * cos(anguloRadianes); // El eje X corre a lo largo de la cancha
    posicionY += distanciaCentro * sin(anguloRadianes); // El eje Y corre a lo ancho de la cancha
}

// Estas funciones se pueden llamar desde Estrategia para obtener la posicion actual del robot
float obtenerCoordenadaX() {return posicionX; } // El eje Y corre a lo ancho de la cancha, con valores positivos hacia la derecha y negativos hacia la izquierda (desde la perspectiva del robot)
float obtenerCoordenadaY() {return posicionY; } // El eje X corre a lo largo de la cancha, con valores positivos hacia el arco contrario y negativos hacia el arco propio (desde la perspectiva del robot)
