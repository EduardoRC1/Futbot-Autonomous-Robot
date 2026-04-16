#include <Arduino.h>
#include "Motores.h"
#include "Sensores.h"
#include "Odometria.h"
#include "ControlPID.h"
#include "Comunicacion.h"
#include "Estrategia.h"
 
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
    
    Serial.println("¡Robot Futbot 100% Online y Listo!");
}

void loop() {
    // 3. Ciclo de pensamiento y movimiento
    actualizarPosicion();
    evaluarEntorno();      
    ejecutarJugadaActual();
    
    revisarConexionSegura(); 
    
    // 4. Monitoreo en pantalla (Opcional, pero muy útil)
    if (hayDatosNuevos()) {
        // Serial.print("📡 CEREBRO ESCUCHA: Pelota = ");
        // Serial.print(datosCamara.balonDetectado);
        // Serial.print(" | Coord X = ");
        // Serial.print(datosCamara.coordX);
        // Serial.print(" | Coord Y = ");
        // Serial.println(datosCamara.coordY);

        // limpiarBanderaDatos(); // Reset the flag
    }
    
    delay(10); 
}