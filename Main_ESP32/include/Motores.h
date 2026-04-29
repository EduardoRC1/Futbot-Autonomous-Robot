// Motores.h
#ifndef MOTORES_H
#define MOTORES_H

#include <Arduino.h>

// Declaración de pines para los drivers BTS7960 
// Usamos 'extern' para decirle al compilador que los valores reales 
// de estos pines se asignarán más adelante en el archivo Motores.cpp
// Cambia esto en tu archivo de cabecera de motores
extern const int MI_R_PWM = 27; 
extern const int MI_L_PWM = 14; 
extern const int MD_R_PWM = 12; 
extern const int MD_L_PWM = 13;

extern const int M2_R_EN;
extern const int M2_L_EN;
extern const int M2_R_PWM;
extern const int M2_L_PWM;

// Funciones principales de locomoción que el equipo programará
void inicializarMotores();
void moverRobot(int velocidadIzquierda, int velocidadDerecha);
void detenerRobot();
void pivotearDerecha(int velocidad);
void pivotearIzquierda(int velocidad);

#endif // MOTORES_H