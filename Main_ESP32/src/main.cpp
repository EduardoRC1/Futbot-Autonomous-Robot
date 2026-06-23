//  main.cpp
// -----------------------------------------------
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
// -----------------------------------------------

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "DualSerial.h"
#include "Config.h"
#include "BusI2C.h"
#include "SensoresToF.h"
#include "SensorIMU.h"
// SensorLinea desactivado (modo sumo, sin línea)
#include "Motores.h"
#include "Odometria.h"
#include "ControlPID.h"
#include "Comunicacion.h"
#include "Estrategia.h"

// Monitor inalámbrico por Bluetooth (app: "Serial Bluetooth Terminal" en Android)
BluetoothSerial SerialBT;

void setup() {
    Serial.begin(115200);
    SerialBT.begin("Futbot_Monitor");
    delay(300);

    dualPrintln("\n Arrancando Eva!");

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
        dualPrintln("AVISO: Ningun sensor ToF respondio — continuando sin ellos");
    }
    dualPrintln("[I2C] Escaneo post-ToF:");
    escanearBusI2C();
    dualPrintln();

    // --- Paso 3: IMU ---
    inicializarIMU();
    dualPrintln();

    // --- Paso 4: Sensor de línea (DESACTIVADO — modo sumo) ---
    dualPrintln("[Linea] DESACTIVADO (modo sumo)");
    dualPrintln();

    // --- Paso 5: Motores ---
    inicializarMotores();
    if (INVERTIR_MOTOR_IZQ || INVERTIR_MOTOR_DER || INTERCAMBIAR_MOTORES)
        dualPrintf("[Motores] Inv I=%d D=%d | Swap=%d\n",
                      INVERTIR_MOTOR_IZQ, INVERTIR_MOTOR_DER, INTERCAMBIAR_MOTORES);
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

    // --- Paso 10: Watchdog — reinicia el ESP32 si el loop se cuelga >3s ---
    esp_err_t wdtErr = esp_task_wdt_init(3, true);
    if (wdtErr == ESP_OK || wdtErr == ESP_ERR_INVALID_STATE) {
        esp_task_wdt_add(NULL);
        dualPrintln("[WDT] Watchdog activo (3s)");
    } else {
        dualPrintf("[WDT] No se pudo activar (err=%d)\n", wdtErr);
    }

    dualPrintln("===== Listo — loop principal =====\n");
}

static unsigned long ultimoDiag = 0;

void loop() {
    // 0. Alimentar watchdog — si el loop se cuelga, ESP32 reinicia
    esp_task_wdt_reset();

    // 0b. Verificar watchdog de motores — si no se actualizan, parar
    verificarWatchdogMotores();

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

        dualPrintf("[DIAG] Estado=%s | ToF F=%s(%umm) I=%s(%umm) D=%s(%umm)"
                 " | Balon=%s dist=%.0f | Cam=%s (msgs=%lu)\n",
                 nombreEstado(obtenerEstadoActual()),
                 opF ? "SI" : "no", obtenerDistanciaFrente(),
                 opI ? "SI" : "no", obtenerDistanciaIzquierda(),
                 opD ? "SI" : "no", obtenerDistanciaDerecha(),
                 datosCamara.balonDetectado ? "SI" : "no",
                 datosCamara.distanciaEstimada,
                 camaraConectada() ? "OK" : "SIN_CONEXION",
                 obtenerContadorMensajes());
    }

    // 7. Pequeña pausa para no saturar el bus
    delay(50);
}
