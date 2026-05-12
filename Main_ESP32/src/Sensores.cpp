#include <Arduino.h>
#include <SPI.h> // Agregamos esto para evitar el error de compilación
#include <Wire.h>
#include "Motores.h"
#include "Sensores.h"
#include "Comunicacion.h"
#include "Odometria.h"
#include "Estrategia.h"

extern MensajeVision datosCamara;

void setup() {
    Serial.begin(115200);
    delay(1500); 
    Serial.println("\n==========================================");
    Serial.println(">>> MODO DIAGNOSTICO: SENSORES I2C <<<");
    Serial.println("==========================================\n");

    // --- SEGURIDAD: MOTORES DESACTIVADOS ---
    // Al comentar esto, el ESP32 no manda señal a los drivers.
    // inicializarMotores(); 
    // detenerRobot(); 

    // 1. Bus I2C
    Serial.println("[1/3] Iniciando Bus I2C...");
    inicializarBusI2C();

    // 2. ToF - Aquí se prueba el puenteo y los XSHUT (5, 32, 15)
    Serial.println("[2/3] Configurando direcciones ToF...");
    inicializarToF_VL53L0X();

    // 3. IMU
    Serial.println("[3/3] Iniciando IMU BNO055...");
    inicializarIMU_BNO055();

    // Sensores extra
    inicializarLinea_QTR8A();
    inicializarRadio();

    // COMENTADOS PARA EVITAR CRASHES POR MEMORIA O I2C
    // inicializarOdometria();
    // inicializarEstrategia();

    Serial.println("\n>>> TEST ACTIVADO: Pasa la mano frente a los sensores <<<");
}

void loop() {
    // Lectura simplificada para monitor serie
    Serial.print("ESTADO: ");

    // Probamos si los ToF ven algo (esto usa el bus I2C)
    if (detectarOponenteFrente()) {
        Serial.print("| OBJETO DETECTADO | ");
    } else {
        Serial.print("| Despejado | ");
    }

    // Probamos el sensor de línea (Analógico)
    if (detectarLineaBlanca()) {
        Serial.print(" [LINEA BLANCA] ");
    }

    // Probamos si llega señal de la cámara
    obtenerDatosCamara();
    if (hayDatosNuevos()) {
        Serial.print(" CamX: ");
        Serial.print(datosCamara.coordX);
        limpiarBanderaDatos();
    }

    Serial.println(""); 
    delay(300); 
}