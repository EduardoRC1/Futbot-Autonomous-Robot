#ifndef MOTORES_H
#define MOTORES_H

// BORRA LOS PINES DE AQUÍ. SOLO DEJA ESTAS FUNCIONES:

void inicializarMotores();
void moverMotores(int velocidadIzquierda, int velocidadDerecha);
void detenerRobot();
void pivotearDerecha(int velocidad);
void pivotearIzquierda(int velocidad);

#endif