#ifndef CONTROLPID_H
#define CONTROLPID_H

// ============================================================================
//  ControlPID.h — Controlador PID para seguimiento de balón
// ============================================================================

#include <Arduino.h>

extern float Kp;
extern float Ki;
extern float Kd;

void inicializarPID();
int calcularVelocidadPID(float errorActual);

#endif // CONTROLPID_H
