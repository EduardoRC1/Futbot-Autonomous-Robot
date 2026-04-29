#include "Motores.h"
#include <Arduino.h>

// --- PINES PARA EL DRIVER BTS7960 (IZQUIERDO - U2) ---
// Según diagrama: Línea verde al 27, Línea roja al 14
const int M1_R_PWM = 27; 
const int M1_L_PWM = 14; 

// --- PINES PARA EL DRIVER BTS7960 (DERECHO - U3) ---
// Según diagrama: Línea verde al 12, Línea roja al 13
const int M2_R_PWM = 12; 
const int M2_L_PWM = 13; 

// NOTA: Los pines EN (Enable) ya no se declaran porque están 
// conectados directamente a 5V en el hardware.

void inicializarMotores() {
    // 1. Configurar el hardware PWM del ESP32 (Frecuencia 5000Hz, Resolucion 8-bit [0-255])
    
    // Motor Izquierdo (M1)
    ledcSetup(0, 5000, 8); 
    ledcAttachPin(M1_R_PWM, 0); // Canal 0: Giro A
    
    ledcSetup(1, 5000, 8); 
    ledcAttachPin(M1_L_PWM, 1); // Canal 1: Giro B
    
    // Motor Derecho (M2)
    ledcSetup(2, 5000, 8); 
    ledcAttachPin(M2_R_PWM, 2); // Canal 2: Giro A
    
    ledcSetup(3, 5000, 8); 
    ledcAttachPin(M2_L_PWM, 3); // Canal 3: Giro B
    
    detenerRobot();
    Serial.println("Motores configurados con pines del diagrama (EN hardwired a 5V).");
}

void moverRobot(int velocidadIzquierda, int velocidadDerecha) {
    // Limitar velocidades por seguridad entre -255 y 255
    velocidadIzquierda = constrain(velocidadIzquierda, -255, 255);
    velocidadDerecha = constrain(velocidadDerecha, -255, 255);

    // --- CONTROL MOTOR IZQUIERDO ---
    if (velocidadIzquierda >= 0) {
        ledcWrite(1, 0);                  // Apagar canal B
        ledcWrite(0, velocidadIzquierda); // Inyectar PWM en canal A
    } else {
        ledcWrite(0, 0);                   // Apagar canal A
        ledcWrite(1, -velocidadIzquierda); // Inyectar PWM en canal B
    }

    // --- CONTROL MOTOR DERECHO ---
    if (velocidadDerecha >= 0) {
        ledcWrite(3, 0);                // Apagar canal B
        ledcWrite(2, velocidadDerecha); // Inyectar PWM en canal A
    } else {
        ledcWrite(2, 0);                 // Apagar canal A
        ledcWrite(3, -velocidadDerecha); // Inyectar PWM en canal B
    }
}

void detenerRobot() {
    ledcWrite(0, 0); 
    ledcWrite(1, 0);
    ledcWrite(2, 0); 
    ledcWrite(3, 0);
}

void pivotearDerecha(int velocidad) { 
    moverRobot(velocidad, -velocidad); 
}

void pivotearIzquierda(int velocidad) { 
    moverRobot(-velocidad, velocidad); 
}