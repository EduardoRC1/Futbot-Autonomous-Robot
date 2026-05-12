#include "ControlPID.h"
#include "Config.h"

float Kp = PID_KP_DEFAULT;
float Ki = PID_KI_DEFAULT;
float Kd = PID_KD_DEFAULT;

static float errorPrevio    = 0.0f;
static float errorAcumulado = 0.0f;

void inicializarPID() {
    errorPrevio    = 0.0f;
    errorAcumulado = 0.0f;
}

int calcularVelocidadPID(float errorActual) {
    float P = Kp * errorActual;

    errorAcumulado = constrain(errorAcumulado + errorActual,
                               -PID_LIMITE_INTEGRAL, PID_LIMITE_INTEGRAL);
    float I = Ki * errorAcumulado;

    float D = Kd * (errorActual - errorPrevio);
    errorPrevio = errorActual;

    return (int)(P + I + D);
}
