// ============================================================================
//  main.cpp — Futbot Autonomous Robot (Defensa)
//  Universidad de Matamoros
//
//  Secuencia de arranque:
//    1. Serial
//    2. Bus I2C (escaneo de diagnóstico)
//    3. Sensores ToF (VL53L0X x3)
//    4. IMU BNO055 (cuando esté conectado)
//    5. Sensor de línea QTR-8A (cuando esté conectado)
//    6. Motores
//    7. Odometría (encoders)
//    8. PID
//    9. Radio ESP-NOW
//   10. Estrategia
// ============================================================================

#include <Arduino.h>
#include "Config.h"
#include "BusI2C.h"
#include "SensoresToF.h"
#include "SensorIMU.h"
#include "SensorLinea.h"
#include "Motores.h"
#include "Odometria.h"
#include "ControlPID.h"
#include "Comunicacion.h"
#include "Estrategia.h"

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println("==============================================");
    Serial.println("  FUTBOT — Sistema de Control v2.0");
    Serial.println("  Universidad de Matamoros");
    Serial.println("==============================================");
    Serial.println();

    // --- Paso 1: Bus I2C ---
    if (!inicializarBusI2C()) {
        Serial.println("FATAL: Bus I2C no disponible");
        while (true) delay(1000);
    }
    Serial.println("[I2C] Escaneo inicial:");
    escanearBusI2C();
    Serial.println();

    // --- Paso 2: Sensores ToF ---
    bool tofOK = inicializarSensoresToF();
    if (!tofOK) {
        Serial.println("AVISO: Ningún sensor ToF respondió — continuando sin ellos");
    }
    Serial.println("[I2C] Escaneo post-ToF:");
    escanearBusI2C();
    Serial.println();

    // --- Paso 3: IMU ---
    bool imuOK = inicializarIMU();
    if (!imuOK) {
        Serial.println("AVISO: BNO055 no respondio - brujula desactivada");
    }
    Serial.println();

    // --- Paso 4: Sensor de linea ---
    inicializarSensorLinea();
    Serial.println();

    // --- Paso 5: Motores ---
    inicializarMotores();
    Serial.println();

    // --- Paso 6: Odometría ---
    inicializarOdometria();
    Serial.println();

    // --- Paso 7: PID ---
    inicializarPID();
    Serial.printf("[PID] Inicializado (Kp=%.2f Ki=%.2f Kd=%.2f)\n", Kp, Ki, Kd);
    Serial.println();

    // --- Paso 8: Radio ---
    inicializarRadio();
    Serial.println();

    // --- Paso 9: Estrategia ---
    inicializarEstrategia();
    Serial.println();

    Serial.println("==============================================");
    Serial.println("  Sistema listo — entrando al loop principal");
    Serial.println("==============================================");
    Serial.println();
}

void loop() {
    static unsigned long ultimoLog = 0;

    // 1. Leer sensores ToF
    LecturasToF lecturas = leerSensoresToF();
    imprimirLecturasToF(lecturas);

    // 2. Actualizar posicion con encoders
    actualizarPosicion();

    // 3. Recibir datos de la camara ESP-NOW
    obtenerDatosCamara();
    revisarConexionSegura();

    // 4. Log de diagnostico cada 2 segundos
    if (millis() - ultimoLog > 2000) {
        ultimoLog = millis();
        float rumbo = leerRumboBrujula();
        bool linea = detectarLineaBlanca();
        uint16_t qtr0 = obtenerValorQTR(0);
        uint16_t qtr1 = obtenerValorQTR(1);
        Serial.printf("[DIAG] Brujula=%.1f | Linea=%s (Q0=%d Q1=%d) | Balon=%s",
                      rumbo,
                      linea ? "SI" : "no", qtr0, qtr1,
                      datosCamara.balonDetectado ? "SI" : "no");
        if (datosCamara.balonDetectado) {
            Serial.printf(" (x=%d y=%d dist=%.1f)",
                          datosCamara.coordX, datosCamara.coordY,
                          datosCamara.distanciaEstimada);
        }
        Serial.println();
    }

    // 5. Decidir que hacer
    evaluarEntorno();

    // 6. Ejecutar la accion decidida
    ejecutarJugadaActual();

    // 7. Pausa para no saturar el bus / serial
    delay(50);
}
