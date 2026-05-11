// ControlPID.cpp
// Universidad de Matamoros — Futbot Autonomous Robot
// Revisado: Anti-windup agregado — errorAcumulado se limita a ±500
//           para evitar que la integral explote cuando Ki > 0.

#include "ControlPID.h"

// ---------------------------------------------------------------------------
// GANANCIAS PID
// Kp: qué tan fuerte reacciona el robot al error actual
// Ki: qué tan fuerte corrige errores acumulados en el tiempo
// Kd: qué tan fuerte frena cuando el error está cambiando rápido
//
// PROCESO DE SINTONIZACIÓN SUGERIDO:
//   1. Empezar con Kp=1.0, Ki=0.0, Kd=0.0
//   2. Subir Kp hasta que el robot siga la pelota pero oscile un poco
//   3. Subir Kd para amortiguar la oscilación
//   4. Solo subir Ki si el robot no llega exactamente al centro — empieza con 0.01
// ---------------------------------------------------------------------------
float Kp = 1.5;
float Ki = 0.0;
float Kd = 0.5;

// Límite del término integral — evita que se acumule sin control
// Si Ki > 0, ajustar este límite según el rango de velocidades (0–255)
const float LIMITE_INTEGRAL = 500.0;

float errorPrevio    = 0.0;
float errorAcumulado = 0.0;

void inicializarPID() {
    errorPrevio    = 0.0;
    errorAcumulado = 0.0;
}

// ---------------------------------------------------------------------------
// calcularVelocidadPID()
// Recibe el error actual (pixels de desviación del centro de la cámara)
// y devuelve un ajuste de velocidad para aplicar a los motores.
//
// Ejemplo: error = +40 (pelota 40px a la derecha)
//   → resultado positivo → motor izq acelera, motor der frena → gira derecha
// ---------------------------------------------------------------------------
int calcularVelocidadPID(float errorActual) {
    // Proporcional
    float P = Kp * errorActual;

    // Integral con anti-windup — constrain evita que se dispare
    errorAcumulado = constrain(errorAcumulado + errorActual, -LIMITE_INTEGRAL, LIMITE_INTEGRAL);
    float I = Ki * errorAcumulado;

    // Derivativo
    float derivada = errorActual - errorPrevio;
    float D        = Kd * derivada;

    errorPrevio = errorActual;

    return (int)(P + I + D);
}