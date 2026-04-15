#include "Motores.h" 

// --- PINES PARA EL DRIVER BTS7960 (IZQUIERDO) ---
const int M1_R_EN = 12;  // Enable Derecho
const int M1_L_EN = 13;  // Enable Izquierdo
const int M1_R_PWM = 14; // PWM Adelante (Canal 0)
const int M1_L_PWM = 27; // PWM Reversa  (Canal 1)

// --- PINES PARA EL DRIVER BTS7960 (DERECHO) ---
const int M2_R_EN = 25;  // Enable Derecho
const int M2_L_EN = 26;  // Enable Izquierdo
const int M2_R_PWM = 15; // (Antes era 32)
const int M2_L_PWM = 23; // (Antes era 33)

void inicializarMotores() {
    // 1. Configurar los pines de Enable (Activación) como salidas
    pinMode(M1_R_EN, OUTPUT); pinMode(M1_L_EN, OUTPUT);
    pinMode(M2_R_EN, OUTPUT); pinMode(M2_L_EN, OUTPUT);
    
    // 2. Encender los drivers permanentemente
    digitalWrite(M1_R_EN, HIGH); digitalWrite(M1_L_EN, HIGH);
    digitalWrite(M2_R_EN, HIGH); digitalWrite(M2_L_EN, HIGH);

    // 3. Configurar el hardware PWM del ESP32 (Frecuencia 5000Hz, Resolucion 8-bit [0-255])
    // Motor Izquierdo
    ledcSetup(0, 5000, 8); ledcAttachPin(M1_R_PWM, 0); // Canal 0: Adelante Izq
    ledcSetup(1, 5000, 8); ledcAttachPin(M1_L_PWM, 1); // Canal 1: Reversa Izq
    
    // Motor Derecho
    ledcSetup(2, 5000, 8); ledcAttachPin(M2_R_PWM, 2); // Canal 2: Adelante Der
    ledcSetup(3, 5000, 8); ledcAttachPin(M2_L_PWM, 3); // Canal 3: Reversa Der
    
    detenerRobot();
}

void moverRobot(int velocidadIzquierda, int velocidadDerecha) {
    // Limitar velocidades por seguridad entre -255 y 255
    velocidadIzquierda = constrain(velocidadIzquierda, -255, 255);
    velocidadDerecha = constrain(velocidadDerecha, -255, 255);

    // --- CONTROL MOTOR IZQUIERDO ---
    if (velocidadIzquierda >= 0) {
        ledcWrite(1, 0);                  // Apagar reversa
        ledcWrite(0, velocidadIzquierda); // Inyectar PWM hacia adelante
    } else {
        ledcWrite(0, 0);                   // Apagar adelante
        ledcWrite(1, -velocidadIzquierda); // Inyectar PWM en reversa (quitando el negativo)
    }

    // --- CONTROL MOTOR DERECHO ---
    if (velocidadDerecha >= 0) {
        ledcWrite(3, 0);                // Apagar reversa
        ledcWrite(2, velocidadDerecha); // Inyectar PWM hacia adelante
    } else {
        ledcWrite(2, 0);                 // Apagar adelante
        ledcWrite(3, -velocidadDerecha); // Inyectar PWM en reversa
    }
}

void detenerRobot() {
    // Frenado total cortando el PWM en los 4 canales
    ledcWrite(0, 0); ledcWrite(1, 0);
    ledcWrite(2, 0); ledcWrite(3, 0);
}

void pivotearDerecha(int velocidad) { 
    moverRobot(velocidad, -velocidad); 
}

void pivotearIzquierda(int velocidad) { 
    moverRobot(-velocidad, velocidad); 
}