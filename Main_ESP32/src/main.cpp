#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// Tus cabeceras de proyecto
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

    // --- MOTORES TOTALMENTE APAGADOS ---
    // No llamamos a inicializarMotores() para asegurar silencio eléctrico.

    // 1. Bus I2C (Pines 21 y 22)
    Serial.println("[1/3] Iniciando Bus I2C...");
    inicializarBusI2C();

    // 2. ToF (Usa los XSHUT 5, 32, 15)
    Serial.println("[2/3] Configurando direcciones ToF...");
    inicializarToF_VL53L0X();

    // 3. IMU (BNO055)
    Serial.println("[3/3] Iniciando IMU BNO055...");
    inicializarIMU_BNO055();

    // Extras
    inicializarLinea_QTR8A();
    inicializarRadio();

    Serial.println("\n>>> SISTEMA LISTO - PASA TU MANO POR LOS SENSORES <<<");
}

void loop() {
    Serial.print("TEST: ");

    // Verificamos oponente (Bus I2C activo)
    if (detectarOponenteFrente()) {
        Serial.print("[ OBJETO! ] ");
    } else {
        Serial.print("[ Despejado ] ");
    }

    // Verificamos línea (Pin analógico)
    if (detectarLineaBlanca()) {
        Serial.print("| LINEA BLANCA |");
    }

    // Datos de la cámara ESP-NOW
    obtenerDatosCamara();
    if (hayDatosNuevos()) {
        Serial.print(" CamX: ");
        Serial.print(datosCamara.coordX);
        limpiarBanderaDatos();
    }

    Serial.println(""); 
    delay(200); 
}