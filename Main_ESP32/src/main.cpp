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
#include "BluetoothSerial.h"
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

// Monitor inalámbrico por Bluetooth (app: "Serial Bluetooth Terminal" en Android)
BluetoothSerial SerialBT;

// Envía un mensaje por USB y por Bluetooth a la vez
static void enviarDual(const char* msg) {
    Serial.print(msg);
    SerialBT.print(msg);
}

void setup() {
    Serial.begin(115200);
    SerialBT.begin("Futbot_Monitor");
    delay(2000);

    Serial.println();
    Serial.println("==============================================");
    Serial.println("  FUTBOT — Sistema de Control v2.0");
    Serial.println("  Universidad de Matamoros");
    Serial.println("==============================================");
    Serial.println("[BT] Monitor Bluetooth activo: 'Futbot_Monitor'");
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
    inicializarIMU();
    Serial.println();

    // --- Paso 4: Sensor de línea ---
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

static unsigned long ultimoDiag = 0;

void loop() {
    // 1. Leer sensores ToF
    LecturasToF lecturas = leerSensoresToF();

    // 2. Actualizar posición con encoders
    actualizarPosicion();

    // 3. Recibir datos de la cámara ESP-NOW
    obtenerDatosCamara();
    revisarConexionSegura();

    // 4. Decidir qué hacer
    evaluarEntorno();

    // 5. Ejecutar la acción decidida
    ejecutarJugadaActual();

    // 6. Diagnóstico cada 500ms
    unsigned long ahora = millis();
    if (ahora - ultimoDiag >= 500) {
        ultimoDiag = ahora;

        imprimirLecturasToF(lecturas);

        bool opF = detectarOponenteFrente();
        bool opI = detectarOponenteIzquierda();
        bool opD = detectarOponenteDerecha();
        bool linea = detectarLineaBlanca();

        char buffer[256];
        snprintf(buffer, sizeof(buffer),
                 "Distancias -> F:%umm | I:%umm | D:%umm\n",
                 obtenerDistanciaFrente(), obtenerDistanciaIzquierda(),
                 obtenerDistanciaDerecha());
        SerialBT.print(buffer);

        snprintf(buffer, sizeof(buffer),
                 "[DIAG] Estado=%s | Oponente F=%s I=%s D=%s | Linea=%s (Q0=%u Q1=%u) | Balon=%s | Cam=%s (msgs=%lu)\n",
                 nombreEstado(obtenerEstadoActual()),
                 opF ? "SI" : "no", opI ? "SI" : "no", opD ? "SI" : "no",
                 linea ? "SI" : "no",
                 obtenerValorQTR(0), obtenerValorQTR(1),
                 datosCamara.balonDetectado ? "SI" : "no",
                 camaraConectada() ? "OK" : "SIN_CONEXION",
                 obtenerContadorMensajes());
        enviarDual(buffer);
    }

    // 7. Pequeña pausa para no saturar el bus
    delay(50);
}
