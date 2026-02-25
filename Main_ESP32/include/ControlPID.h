#ifndef CONTROLPID_H
#define CONTROLPID_H

#include <Arduino.h>

// Variables para sintonizar el comportamiento del robot en la cancha
extern float Kp; // Proporcional (fuerza de empuje)
extern float Ki; // Integral (correcion de errores pasados)
extern float Kd; // Derivativo (freno para evitar oscilar)

void inicializarPID(); // Esta función inicializa las variables necesarias para el cálculo PID, como el error previo y el error acumulado
int calcularVelocidadPID(float errorActual); // Esta función toma el error actual (por ejemplo, la desviación del robot respecto a la línea) y devuelve un ajuste de velocidad basado en el cálculo PID

#endif // CONTROLPID_H