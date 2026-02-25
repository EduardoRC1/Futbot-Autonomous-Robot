// Este es el archivo principal del programa
// Aquí se inicializan todos los subsistemas y se ejecuta el ciclo principal del robot
#include <Arduino.h>
#include "Motores.h"
#include "Sensores.h"
#include "Odometria.h"
#include "ControlPID.h"
#include "Comunicacion.h"
#include "Estrategia.h"

void setup() {
    Serial.begin(115200);
    
    // Inicializar todos los subsistemas físicos de forma ordenada
    inicializarMotores(); // Este modulo vive en Motores.cpp
    inicializarBusI2C(); // Este modulo vive en Sensores.cpp
    inicializarIMU_BNO055(); // Este modulo vive en Sensores.cpp
    inicializarToF_VL53L0X(); // Este modulo vive en Sensores.cpp
    inicializarLinea_QTR8A(); // Este modulo vive en Sensores.cpp
    
    // Inicializar los subsistemas de inteligencia y red
    inicializarOdometria(); // Este modulo vive en Odometria.cpp
    inicializarPID(); // Este modulo vive en ControlPID.cpp
    inicializarRadio(); // Este modulo vive en Comunicacion.cpp
    inicializarEstrategia(); // Este modulo vive en Estrategia.cpp
    
    // Estos println solamente son para verificar que se este ejecutando el setup correctamente
    // Luego se puede eliminar o comentar para ahorrar recursos
    Serial.println("Robot Defensor Listo! Esperando...");
}

void loop() {
    // 1. Actualizar el GPS interno (saber dónde estamos)
    actualizarPosicion();
    
    // 2. El director técnico piensa (lee sensores y cámara)
    evaluarEntorno();
    
    // 3. El director técnico actúa (mueve los motores)
    ejecutarJugadaActual();
    
    delay(10); // Pequeña pausa para darle estabilidad al procesador
}
