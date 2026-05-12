#include "Motores.h"
#include "Config.h"
#include <Arduino.h>

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
    Serial.println("[Motores] Inicializados");
}

void moverMotores(int velocidadIzquierda, int velocidadDerecha) {
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
