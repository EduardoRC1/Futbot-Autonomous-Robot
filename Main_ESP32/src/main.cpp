
// Codigo principal de nuestro proyecto, para una visualizacion 
//sobre el flujo del robot --> docs/Diagrama_Flujo_Robot.pdf 

#include <Arduino.h>
#include "Motores.h"
#include "Sensores.h"
#include "Odometria.h"
#include "ControlPID.h"
#include "Comunicacion.h"
#include "Estrategia.h"

extern EstadoRobot estadoActual;
extern MensajeVision datosCamara;

// --- FUNCION DE TELEMETRIA (Monitoreo sin bloquear el procesador) ---
void imprimirTelemetria() {
    // 4. Monitoreo de Sensores Crudos (Modo Debug)
    static unsigned long ultimoPrintSensores = 0;
    
    // Imprime el reporte solo cada 500 milisegundos (2 veces por segundo)
    if (millis() - ultimoPrintSensores > 500) { 
        Serial.println("--- REPORTE DE SENSORES ---");
        
        // 1. Suelo (QTR-8A)
        Serial.print("🚥 SUELO: Línea Blanca = ");
        Serial.println(detectarLineaBlanca() ? "¡PELIGRO (SI)!" : "Seguro (NO)");

        // 2. Láser Frontal (VL53L0X)
        Serial.print("📏 LÁSER: Rival Cerca = ");
        Serial.println(detectarOponenteFrente() ? "¡OBSTÁCULO (SI)!" : "Despejado (NO)");

        // 3. Radio / Cámara (ESP-NOW)
        if (hayDatosNuevos()) {
            Serial.print("📸 CÁMARA: Pelota="); 
            Serial.print(datosCamara.balonDetectado ? "VISTA" : "PERDIDA");
            Serial.print(" | X="); 
            Serial.print(datosCamara.coordX);
            Serial.print(" | Distancia="); 
            Serial.println(datosCamara.distanciaEstimada);
            
            limpiarBanderaDatos(); // Resetea la bandera de nuevo mensaje
        } else {
            Serial.println("📸 CÁMARA: Esperando conexión/datos...");
        }
        
        Serial.println("---------------------------");
        ultimoPrintSensores = millis(); // Reinicia el temporizador
    }
}

void setup() {
    Serial.begin(115200);
    
    // 1. Inicializar hardware físico
    inicializarMotores(); 
    inicializarBusI2C(); 
    inicializarIMU_BNO055(); 
    inicializarToF_VL53L0X(); 
    inicializarLinea_QTR8A(); 
    
    // 2. Inicializar Inteligencia y Red
    inicializarOdometria();  
    inicializarPID();        
    inicializarRadio(); // Enciende ESP-NOW
    inicializarEstrategia(); // ¡Estrategia encendida!
    
    Serial.println("Robot Futbot Listo! Iniciando escaneo del entorno...");
}

void loop() {
    // 3. Ciclo de pensamiento y movimiento
    actualizarPosicion();
    evaluarEntorno();      
    ejecutarJugadaActual();
    
    revisarConexionSegura(); 
    
    // 4. (Opcional) Leer datos crudos de la cámara si hay un error de posición
    if (hayDatosNuevos()) {
        // Descomenta esto solo si necesitas ver las coordenadas exactas de los píxeles
        Serial.print("📡 CEREBRO ESCUCHA: Pelota = ");
        Serial.print(datosCamara.balonDetectado);
        Serial.print(" | Coord X = ");
        Serial.print(datosCamara.coordX);
        Serial.print(" | Coord Y = ");
        Serial.println(datosCamara.coordY);

        // limpiarBanderaDatos(); // Reset the flag
    }
    
    // 5. Monitoreo general en pantalla de la máquina de estados
    imprimirTelemetria();
    
    delay(10); // Pausa de 10ms para estabilidad del ciclo
}