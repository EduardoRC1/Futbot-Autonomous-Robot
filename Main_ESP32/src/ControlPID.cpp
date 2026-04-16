// ControlPID.cpp, aqui se implementara la logica del ControlPID(Proporcional, Integral, Derivativo) para ajustar las velocidades 
// de los motores en base a la posicion del balon detectada por la camara
#include "ControlPID.h"
// Los siguientes valores solo son ejemplos, tendran que ser revisados y ajustados

float Kp = 1.5; // Ganacia Proporcional
float Ki = 0.0; // Ganancia Integral
float Kd = 0.5; // Ganancia Derivativa

float errorPrevio = 0;
float errorAcumulado = 0;

void inicializarPID() {
    errorPrevio = 0;
    errorAcumulado = 0;
}

int calcularVelocidadPID(float errorActual) {
    // Proporcional
    float P = Kp * errorActual;

    // Integral
    errorAcumulado += errorActual;
    float I = Ki * errorAcumulado;

    // Derivativo
    float derivada = errorActual - errorPrevio;
    float D = Kd * derivada;

    errorPrevio = errorActual;
    return (int)(P + I + D);
}

