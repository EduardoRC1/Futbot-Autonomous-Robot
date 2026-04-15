// Odometria.h
// Este modulo leera los encoders para saber exactamente en que coordenadas esta el robot
#ifndef ODOMETRIA_H
#define ODOMETRIA_H

#include <Arduino.h>

// Pines de interrupcion para los encoders de los motores
extern const int ENCODER_IZQ_A; //motor izquierdo canal A
extern const int ENCODER_DER_A; //motor derecho canal A

void inicializarOdometria(); // Esta funcion configurara los pines de los encoders y las interrupciones necesarias para leerlos correctamente
void actualizarPosicion(); // Esta funcion se llamara dentro de las interrupciones de los encoders para actualizar la posicion del robot cada vez que se detecte un cambio en los encoders
float obtenerCoordenadaX(); // Esta funcion devolvera la coordenada X actual del robot
float obtenerCoordenadaY(); // Esta funcion devolvera la coordenada Y actual del robot

#endif // ODOMETRIA_H
