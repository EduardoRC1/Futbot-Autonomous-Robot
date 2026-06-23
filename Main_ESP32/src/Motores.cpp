#include "Motores.h"
#include "Config.h"
#include "DualSerial.h"
#include <Arduino.h>

static const unsigned long MOTOR_WATCHDOG_MS = 250;
static unsigned long ultimoComandoMotor = 0;

void inicializarMotores() {
    ledcSetup(PWM_CH_M1_R, PWM_FREQ, PWM_RESOLUCION);
    ledcAttachPin(PIN_M1_R_PWM, PWM_CH_M1_R);

    ledcSetup(PWM_CH_M1_L, PWM_FREQ, PWM_RESOLUCION);
    ledcAttachPin(PIN_M1_L_PWM, PWM_CH_M1_L);

    ledcSetup(PWM_CH_M2_R, PWM_FREQ, PWM_RESOLUCION);
    ledcAttachPin(PIN_M2_R_PWM, PWM_CH_M2_R);

    ledcSetup(PWM_CH_M2_L, PWM_FREQ, PWM_RESOLUCION);
    ledcAttachPin(PIN_M2_L_PWM, PWM_CH_M2_L);

    detenerRobot();
    dualPrintln("[Motores] Inicializados");
}

void verificarWatchdogMotores() {
    if (ultimoComandoMotor > 0 &&
        (millis() - ultimoComandoMotor > MOTOR_WATCHDOG_MS)) {
        detenerRobot();
    }
}

void moverMotores(int velocidadIzquierda, int velocidadDerecha) {
    ultimoComandoMotor = millis();

    if (INTERCAMBIAR_MOTORES) {
        int tmp = velocidadIzquierda;
        velocidadIzquierda = velocidadDerecha;
        velocidadDerecha = tmp;
    }
    if (INVERTIR_MOTOR_IZQ) velocidadIzquierda = -velocidadIzquierda;
    if (INVERTIR_MOTOR_DER) velocidadDerecha   = -velocidadDerecha;

    velocidadIzquierda = constrain(velocidadIzquierda, -255, 255);
    velocidadDerecha   = constrain(velocidadDerecha,   -255, 255);

    if (velocidadIzquierda >= 0) {
        ledcWrite(PWM_CH_M1_L, 0);
        ledcWrite(PWM_CH_M1_R, velocidadIzquierda);
    } else {
        ledcWrite(PWM_CH_M1_R, 0);
        ledcWrite(PWM_CH_M1_L, -velocidadIzquierda);
    }

    if (velocidadDerecha >= 0) {
        ledcWrite(PWM_CH_M2_L, 0);
        ledcWrite(PWM_CH_M2_R, velocidadDerecha);
    } else {
        ledcWrite(PWM_CH_M2_R, 0);
        ledcWrite(PWM_CH_M2_L, -velocidadDerecha);
    }
}

void detenerRobot() {
    ledcWrite(PWM_CH_M1_R, 0);
    ledcWrite(PWM_CH_M1_L, 0);
    ledcWrite(PWM_CH_M2_R, 0);
    ledcWrite(PWM_CH_M2_L, 0);
}

void pivotearDerecha(int velocidad)   { moverMotores( velocidad, -velocidad); }
void pivotearIzquierda(int velocidad) { moverMotores(-velocidad,  velocidad); }

// Giro suave: el motor del lado hacia donde gira se detiene, el otro avanza.
// Derecha → motor derecho parado, motor izquierdo avanza.
void girarSuaveDerecha(int velocidad)   { moverMotores( velocidad, 0); }
void girarSuaveIzquierda(int velocidad) { moverMotores(0,  velocidad); }
