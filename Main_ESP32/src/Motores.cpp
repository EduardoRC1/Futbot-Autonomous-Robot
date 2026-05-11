#include "Motores.h"
#include <Arduino.h>

// --- PINES CORREGIDOS SEGÚN DIAGRAMA ---
// BTS Izquierdo (M1)
const int M1_R_PWM = 26; 
const int M1_L_PWM = 25; 

// BTS Derecho (M2)
const int M2_R_PWM = 14; 
const int M2_L_PWM = 27; 

void inicializarMotores() {
    // Configuración PWM ESP32 (5000Hz, 8-bit)
    ledcSetup(0, 5000, 8); ledcAttachPin(M1_R_PWM, 0); 
    ledcSetup(1, 5000, 8); ledcAttachPin(M1_L_PWM, 1); 
    
    ledcSetup(2, 5000, 8); ledcAttachPin(M2_R_PWM, 2); 
    ledcSetup(3, 5000, 8); ledcAttachPin(M2_L_PWM, 3); 
    
    detenerRobot();
    Serial.println("Motores inicializados con pines del diagrama (26,25,14,27).");
}

void moverMotores(int velocidadIzquierda, int velocidadDerecha) {
    velocidadIzquierda = constrain(velocidadIzquierda, -255, 255);
    velocidadDerecha = constrain(velocidadDerecha, -255, 255);

    // CONTROL MOTOR IZQUIERDO
    if (velocidadIzquierda >= 0) {
        ledcWrite(1, 0);                  
        ledcWrite(0, velocidadIzquierda); 
    } else {
        ledcWrite(0, 0);                  
        ledcWrite(1, abs(velocidadIzquierda)); 
    }

    // CONTROL MOTOR DERECHO
    if (velocidadDerecha >= 0) {
        ledcWrite(3, 0);                  
        ledcWrite(2, velocidadDerecha); 
    } else {
        ledcWrite(2, 0);                  
        ledcWrite(3, abs(velocidadDerecha)); 
    }
}

void detenerRobot() {
    ledcWrite(0, 0); ledcWrite(1, 0);
    ledcWrite(2, 0); ledcWrite(3, 0);
}

void pivotearDerecha(int velocidad) { moverMotores(velocidad, -velocidad); }
void pivotearIzquierda(int velocidad) { moverMotores(-velocidad, velocidad); }