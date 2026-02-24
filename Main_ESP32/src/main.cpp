#include <Arduino.h>
#include "Motores.h"
#include "Sensores.h"
#include "ProtocoloEspNow.h"

// ==========================================
// 1. SETUP (Runs once at boot)
// ==========================================
void setup() {
    Serial.begin(115200);
    Serial.println("Futbot 2026 - Cerebro Central Iniciando...");

    // Call the setup subroutines written by your other programmers
    inicializarMotores();
    inicializarBusI2C();
    inicializarIMU_BNO055();
    
    Serial.println("Sistemas listos. Esperando silbato...");
}

// ==========================================
// 2. MAIN LOOP (The continuous decision cycle)
// ==========================================
void loop() {
    // Step 1: Read the environment
    bool peligroChoque = detectarOponenteFrente();
    bool limiteCancha = detectarLineaBlanca();
    
    // Step 2: Make decisions based on the sensors
    if (limiteCancha) {
        // If we are about to cross the white line, stop and pivot!
        detenerRobot();
        pivotearDerecha(200); 
    } 
    else if (peligroChoque) {
        // If the ToF laser sees an opponent, stop to avoid a penalty
        detenerRobot();
    } 
    else {
        // Step 3: Normal gameplay logic goes here
        // (e.g., If the ESP-NOW camera says the ball is ahead, drive forward)
        moverRobot(255, 255); 
    }

    // A tiny delay to keep the scan cycle stable
    delay(10); 
}