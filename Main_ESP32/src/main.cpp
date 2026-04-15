#include <Arduino.h>
#include "Motores.h"
#include "Sensores.h"
#include "Odometria.h"
#include "ControlPID.h"
#include "Comunicacion.h"
#include "Estrategia.h"

void setup() {
    Serial.begin(115200);
    
    // 1. Inicializar todo el hardware Físico
    inicializarMotores(); 
    inicializarBusI2C(); 
    inicializarIMU_BNO055(); 
    inicializarToF_VL53L0X(); 
    inicializarLinea_QTR8A(); 
    
    // 2. Inicializar Inteligencia y Red
    inicializarOdometria();  
    inicializarPID();        
    inicializarRadio(); // Enciende ESP-NOW en Canal 1
    inicializarEstrategia(); 
    
    Serial.println("¡Robot Futbot Listo!");
}

void loop() {
    actualizarPosicion();
    evaluarEntorno();
    ejecutarJugadaActual();
    revisarConexionSegura();
    
    delay(10); // Un delay de 10ms al final del loop para no saturar el microcontrolador
}