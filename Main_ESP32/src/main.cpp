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
#include "DualSerial.h"
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
    inicializarDualSerial();
    delay(2000);

    dualPrintln();
    dualPrintln("==============================================");
    dualPrintln("  FUTBOT — Sistema de Control v2.0");
    dualPrintln("  Universidad de Matamoros");
    dualPrintln("==============================================");
    dualPrintln("[BT] Monitor Bluetooth activo: 'Futbot_Monitor'");
    dualPrintln();

    // --- Paso 1: Bus I2C ---
    if (!inicializarBusI2C()) {
        dualPrintln("FATAL: Bus I2C no disponible");
        while (true) delay(1000);
    }
    dualPrintln("[I2C] Escaneo inicial:");
    escanearBusI2C();
    dualPrintln();

    // --- Paso 2: Sensores ToF ---
    bool tofOK = inicializarSensoresToF();
    if (!tofOK) {
        dualPrintln("AVISO: Ningún sensor ToF respondió — continuando sin ellos");
    }
    dualPrintln("[I2C] Escaneo post-ToF:");
    escanearBusI2C();
    dualPrintln();

    // --- Paso 3: IMU ---
    inicializarIMU();
    dualPrintln();

    // --- Paso 4: Sensor de línea ---
    inicializarSensorLinea();
    dualPrintln();

    // --- Paso 5: Motores ---
    inicializarMotores();
    dualPrintln();

    // --- Paso 6: Odometría ---
    inicializarOdometria();
    dualPrintln();

    // --- Paso 7: PID ---
    inicializarPID();
    dualPrintf("[PID] Inicializado (Kp=%.2f Ki=%.2f Kd=%.2f)\n", Kp, Ki, Kd);
    dualPrintln();

    // --- Paso 8: Radio ---
    inicializarRadio();
    dualPrintln();

    // --- Paso 9: Estrategia ---
    inicializarEstrategia();
    dualPrintln();

    dualPrintln("==============================================");
    dualPrintln("  Sistema listo — entrando al loop principal");
    dualPrintln("==============================================");
    dualPrintln();
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

        dualPrintf("[DIAG] Estado=%s | Oponente F=%s I=%s D=%s | Linea=%s (Q0=%u Q1=%u)"
                 " | Balon=%s dist=%.0f Port=%s | Cam=%s (msgs=%lu)\n",
                 nombreEstado(obtenerEstadoActual()),
                 opF ? "SI" : "no", opI ? "SI" : "no", opD ? "SI" : "no",
                 linea ? "SI" : "no",
                 obtenerValorQTR(0), obtenerValorQTR(1),
                 datosCamara.balonDetectado ? "SI" : "no",
                 datosCamara.distanciaEstimada,
                 datosCamara.porteriaEnemigaAlineada ? "SI" : "no",
                 camaraConectada() ? "OK" : "SIN_CONEXION",
                 obtenerContadorMensajes());
    }

    // 7. Pequeña pausa para no saturar el bus
    delay(50);
}
