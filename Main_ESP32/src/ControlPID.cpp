#include "ControlPID.h"

// Los siguientes valores son ejemplos, tendran que ser revisados y ajustados

float Kp = 1.5;
float Ki = 0.0;
float Kd = 0.5;

float errorPrevio = 0;
float errorAcumulado = 0;

void inicilizarPID() {
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

    // Calcular el ajuste total del PWM(Pulse Width Modulation)
    int ajusteVelocidad = (int)(P + I + D);
    return ajusteVelocidad;
}
