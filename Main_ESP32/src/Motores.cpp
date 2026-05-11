#include "Motores.h"
#include <Arduino.h>

// --- PINES PARA EL DRIVER BTS7960 ---
const int M1_R_PWM = 27; 
const int M1_L_PWM = 14; 
const int M2_R_PWM = 12; 
const int M2_L_PWM = 13; 

void inicializarMotores() {
    // Configuración PWM ESP32
    ledcSetup(0, 5000, 8); 
    ledcAttachPin(M1_R_PWM, 0); 
    
    ledcSetup(1, 5000, 8); 
    ledcAttachPin(M1_L_PWM, 1); 
    
    ledcSetup(2, 5000, 8); 
    ledcAttachPin(M2_R_PWM, 2); 
    
    ledcSetup(3, 5000, 8); 
    ledcAttachPin(M2_L_PWM, 3); 
    
    detenerRobot();
    Serial.println("Motores listos.");
}

// CORRECCIÓN: Cambiado de moverRobot a moverMotores
void moverMotores(int velocidadIzquierda, int velocidadDerecha) {
    velocidadIzquierda = constrain(velocidadIzquierda, -255, 255);
    velocidadDerecha = constrain(velocidadDerecha, -255, 255);

    // --- MOTOR IZQUIERDO ---
    if (velocidadIzquierda >= 0) {
        ledcWrite(1, 0);                  // Apagar reversa
        ledcWrite(0, velocidadIzquierda); // Avanzar
    } else {
        ledcWrite(0, 0);                  // Apagar avance
        ledcWrite(1, abs(velocidadIzquierda)); // Reversa (valor positivo)
    }

    // --- MOTOR DERECHO ---
    if (velocidadDerecha >= 0) {
        ledcWrite(3, 0);                  // Apagar reversa
        ledcWrite(2, velocidadDerecha);   // Avanzar
    } else {
        ledcWrite(2, 0);                  // Apagar avance
        ledcWrite(3, abs(velocidadDerecha));   // Reversa (valor positivo)
    }
}

void detenerRobot() {
    ledcWrite(0, 0); 
    ledcWrite(1, 0);
    ledcWrite(2, 0); 
    ledcWrite(3, 0);
}

// Estas también deben llamar a moverMotores
void pivotearDerecha(int velocidad) { 
    moverMotores(velocidad, -velocidad); 
}

void pivotearIzquierda(int velocidad) { 
    moverMotores(-velocidad, velocidad); 
}