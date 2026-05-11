// main.cpp
#include <Arduino.h>
#include "Motores.h"
#include "Sensores.h"
#include "Comunicacion.h"
#include "Odometria.h"
#include "Estrategia.h"

extern MensajeVision datosCamara;

// ---------------------------------------------------------------------------
// SETUP — se ejecuta una sola vez al encender
// Orden crítico:
//   1. Motores  → primero para que el robot no salga disparado al arrancar
//   2. I2C bus  → debe estar listo antes de cualquier sensor
//   3. ToF lasers → asignan direcciones I2C únicas (ver Sensores.cpp)
//   4. BNO055   → después de los láseres para no reiniciar el bus
//   5. QTR      → analógico, no depende de I2C
//   6. Radio    → ESP-NOW independiente del I2C
//   7. Estrategia → lógica de juego al último
// ---------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    Serial.println(">>> SISTEMA DE COMBATE FUTBOT V1.0 <<<");

    // 1. Motores
    inicializarMotores();

    // 2. Bus I2C (SDA=21, SCL=22 @ 400kHz)
    inicializarBusI2C();

    // 3. Sensores I2C — orden importante (ver nota en Sensores.cpp)

    // =======AQUI SE COMENTAN PARA HACER PRUEBA=============

    // inicializarToF_VL53L0X();   // Láseres primero — asignan direcciones únicas
    // inicializarIMU_BNO055();    // Brújula después — no reinicia el bus

    // =============AQUI SE DESCOMENTAN PARA HACER PRUEBA=============

    // 4. Sensor de línea (analógico, independiente)
    inicializarLinea_QTR8A();

    // 5. Radio ESP-NOW
    inicializarRadio();

    // 6. Odometría (encoders)
    // =========IGUAL SE COMENTARA==============
    
    // inicializarOdometria();

    // =========IGUAL SE DESCOMENTARA==============
    
    // 7. Lógica de juego
    inicializarEstrategia();

    Serial.println(">>> ROBOT EN LINEA - ESPERANDO INICIO <<<");

    // --- GIRO DE CONFIRMACIÓN 360 ---
    // NOTA TORNEO: Ajusta el tiempo según nivel de batería.
    // A batería llena ~750ms, a batería baja ~900ms.
    // Para mayor precisión, reemplazar con giro basado en brújula.
    Serial.println("Giro de confirmacion iniciado...");
    moverMotores(180, -180);
    delay(800);
    detenerRobot();
    delay(500);

    Serial.println(">>> ROBOT LISTO <<<");
}

// ---------------------------------------------------------------------------
// LOOP — ciclo principal de juego
//
// Prioridades (de mayor a menor):
//   A. Línea blanca  → evasión inmediata, nada más importa
//   B. Oponente cerca → embestir (solo si no hay línea)
//   C. Pelota visible → seguir con cámara (solo si no hay línea ni oponente)
//   D. Sin datos      → girar buscando
//
// FIX: El bloque de oponente ahora está dentro del else, así que nunca
//      sobreescribe una evasión de línea activa.
// FIX: Los delays de evasión de línea son no-bloqueantes usando millis().
// ---------------------------------------------------------------------------

// Temporizador de evasión de línea (no-bloqueante)
static unsigned long tiempoEvasionLinea  = 0;
static bool          estandoEvadiendo    = false;
static uint8_t       faseEvasion         = 0; // 0=reversa, 1=giro

void loop() {

    // Actualizar posición por encoders cada ciclo
    actualizarPosicion();

    // Copia segura de datos de cámara desde el buffer del callback ESP-NOW
    obtenerDatosCamara();

    // A. EVASIÓN DE LÍNEA — Máxima prioridad, no-bloqueante
    if (estandoEvadiendo) {
        unsigned long ahora = millis();

        if (faseEvasion == 0) {
            // Fase 0: reversa por 300ms
            moverMotores(-200, -200);
            if (ahora - tiempoEvasionLinea >= 300) {
                faseEvasion = 1;
                tiempoEvasionLinea = ahora; // reiniciar timer para fase 1
            }
        } else {
            // Fase 1: giro por 200ms
            moverMotores(150, -150);
            if (ahora - tiempoEvasionLinea >= 200) {
                // Evasión completa
                estandoEvadiendo = false;
                faseEvasion = 0;
                detenerRobot();
            }
        }
        // Mientras evadimos no ejecutamos ninguna otra lógica
        delay(10);
        return;
    }

    // Detectar nueva línea blanca → iniciar secuencia de evasión
    if (detectarLineaBlanca()) {
        Serial.println("!!! LINEA DETECTADA - EVADIENDO !!!");
        estandoEvadiendo   = true;
        faseEvasion        = 0;
        tiempoEvasionLinea = millis();
        delay(10);
        return;
    }

    // B & C. LÓGICA DE JUEGO (solo si no hay línea)
    // FIX: el bloque de oponente está aquí dentro, NO después del else,
    //      así que nunca pisará una evasión de línea.

    // B. Oponente frente — prioridad sobre la cámara
    if (detectarOponenteFrente()) {
        Serial.println("Oponente detectado - EMBESTIR");
        moverMotores(255, 255);
    }
    // C. Lógica de cámara — solo si no hay oponente inmediato
    else if (hayDatosNuevos()) {
        revisarConexionSegura(); // Invalida datos si llevan >500ms sin actualizarse

        if (datosCamara.balonDetectado) {
            Serial.println("Pelota vista - ATACANDO");
            // Seguir coordenada X de la cámara (resolución 320x240, centro = 160)
            if (datosCamara.coordX < 140) {
                // Pelota a la izquierda — curvar izquierda
                moverMotores(100, 180);
            } else if (datosCamara.coordX > 180) {
                // Pelota a la derecha — curvar derecha
                moverMotores(180, 100);
            } else {
                // Pelota al centro — avanzar directo
                moverMotores(255, 255);
            }
        } else {
            // Cámara conectada pero no ve la pelota — girar buscando
            moverMotores(120, -120);
        }

        limpiarBanderaDatos();
    }
    // D. Sin datos de cámara — girar buscando
    else {
        moverMotores(120, -120);
    }

    delay(10);
}