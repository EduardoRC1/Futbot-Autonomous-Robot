#include <Arduino.h>
#include "Sensores.h"

void setup() {
    // La velocidad debe ser 115200 para que coincida con PlatformIO
    Serial.begin(115200);
    delay(2000); 
    
    Serial.println("\n--- REINICIO DE SISTEMA: BYPASS PINES 16/17 ---");

    // 1. Arrancamos el bus en los pines nuevos
    inicializarBusI2C();
    
    // 2. Encendemos y damos dirección a los ToF uno por uno
    inicializarToF_VL53L0X();
    
    Serial.println("Sistema listo. Esperando detección...");
}

void loop() {
    // Leemos las distancias y las mandamos al monitor
    obtenerLecturasDetalladas();
    
    // Un delay pequeño para no saturar la terminal
    delay(100); 
}