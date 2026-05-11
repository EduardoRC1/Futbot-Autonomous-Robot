#include <Arduino.h>
#include "Motores.h"
#include "Sensores.h"
#include "Comunicacion.h"
#include "Estrategia.h"

// Variables globales que vienen de otros archivos
extern MensajeVision datosCamara; 

void setup() {
    Serial.begin(115200);
    Serial.println(">>> SISTEMA DE COMBATE FUTBOT V1.0 <<<");

    // 1. Hardware Crítico
    inicializarMotores(); 
    inicializarBusI2C(); // Configurado a pines 21 y 22
    
    // 2. Sensores (Si fallan, el código sigue)
    inicializarToF_VL53L0X(); 
    inicializarLinea_QTR8A(); 
    
    // 3. Comunicación con la Cámara (ESP-NOW)
    // Esto es vital porque no depende del I2C
    inicializarRadio(); 
    
    // 4. Lógica de Juego
    inicializarEstrategia(); 

    Serial.println(">>> ROBOT EN LINEA - ESPERANDO INICIO <<<");
    
    // --- GIRO DE CONFIRMACIÓN 360 ---
    Serial.println("Giro de prueba iniciado...");
    moverMotores(180, -180); // Empieza a girar sobre su eje
    delay(800);              // Ajusta este tiempo (ms) para que sea un giro completo
    detenerRobot();          // Se detiene un momento antes de empezar a pelear
    delay(500);
    
    Serial.println("Robot listo y encendido.");
}

void loop() {
    // A. REVISAR SEGURIDAD (Línea Blanca)
    // Si detectamos borde, retroceder es prioridad máxima
    if (detectarLineaBlanca()) {
        Serial.println("!!! LINEA DETECTADA - EVADIENDO !!!");
        moverMotores(-200, -200);
        delay(300);
        moverMotores(150, -150); // Giro rápido
        delay(200);
    } 
    else {
        // B. LÓGICA DE ATAQUE (Basada en Cámara)
        if (hayDatosNuevos()) {
            // Si la cámara ve la pelota, ir tras ella
            if (datosCamara.balonDetectado) {
                Serial.println("Pelota vista - ATACANDO");
                // Lógica simple: Seguir la coordenada X
                if (datosCamara.coordX < 140) { // Pelota a la izquierda
                    moverMotores(100, 180);
                } else if (datosCamara.coordX > 180) { // Pelota a la derecha
                    moverMotores(180, 100);
                } else { // Pelota al centro
                    moverMotores(255, 255); // ¡FULL POWER!
                }
            } else {
                // Si no ve la pelota, girar para buscarla
                moverMotores(120, -120);
            }
            limpiarBanderaDatos();
        } 
        
        // C. LÓGICA DE OPONENTE (Láseres)
        // Si el láser de frente detecta algo, ignorar cámara y embestir
        if (detectarOponenteFrente()) {
            Serial.println("Oponente detectado - EMBESTIR");
            moverMotores(255, 255);
        }
    }

    // Pequeña pausa para estabilidad
    delay(10); 
}