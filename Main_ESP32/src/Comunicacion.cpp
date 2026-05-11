// Comunicacion.cpp
// Universidad de Matamoros — Futbot Autonomous Robot
// Revisado: volatile agregado a variables compartidas entre Core 0 (ESP-NOW)
//           y Core 1 (loop). datosCamara se copia de forma segura con
//           noInterrupts()/interrupts() para evitar lecturas a mitad de escritura.

#include <Arduino.h>
#include "Comunicacion.h"
#include "ProtocoloEspNow.h"
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// ---------------------------------------------------------------------------
// VARIABLES COMPARTIDAS ENTRE CORES
//
// El callback alRecibirDatos() corre en Core 0 (tarea WiFi del ESP32).
// El loop() corre en Core 1.
// Sin volatile, el compilador puede cachear estos valores en un registro
// y loop() nunca vería las actualizaciones del callback.
//
// datosCamara es un struct — volatile en el puntero no protege lecturas
// parciales de sus campos. Usamos un buffer interno + noInterrupts() al
// copiarlo hacia afuera para garantizar una lectura atómica completa.
// ---------------------------------------------------------------------------

// Buffer interno — solo escribe el callback
static volatile MensajeVision bufferCamara;

// Variable pública que leen los demás módulos — se actualiza en obtenerDatosCamara()
MensajeVision datosCamara;

volatile bool          datosNuevosRecibidos = false;
volatile unsigned long tiempoUltimoMensaje  = 0;

// ---------------------------------------------------------------------------
// 1. CALLBACK ESP-NOW — se ejecuta en Core 0 al recibir un paquete
// ---------------------------------------------------------------------------
void alRecibirDatos(const uint8_t *mac, const uint8_t *datosEntrantes, int len) {
    // Verificar que el paquete venga de la MAC de nuestra cámara
    if (memcmp(mac, direccionMacCamara, 6) != 0) return;

    // Verificar tamaño correcto para evitar desbordamiento de buffer
    if (len != sizeof(MensajeVision)) return;

    // Copiar al buffer interno (Core 0, sin riesgo de colisión aquí)
    memcpy((void*)&bufferCamara, datosEntrantes, sizeof(MensajeVision));

    datosNuevosRecibidos = true;
    tiempoUltimoMensaje  = millis();
}

// ---------------------------------------------------------------------------
// 2. FUNCIÓN DE SEGURIDAD — llamar desde loop() cada ciclo
//    Si pasan más de 500ms sin mensaje, consideramos que la cámara se perdió
//    y apagamos la bandera de balón para que el robot no persiga fantasmas.
// ---------------------------------------------------------------------------
void revisarConexionSegura() {
    if (millis() - tiempoUltimoMensaje > 500) {
        datosCamara.balonDetectado = false;
    }
}

// ---------------------------------------------------------------------------
// 3. COPIA SEGURA — llamar desde loop() antes de leer datosCamara
//    noInterrupts() pausa brevemente el scheduler para que la copia
//    no quede a mitad si el callback dispara en Core 0 al mismo tiempo.
// ---------------------------------------------------------------------------
void obtenerDatosCamara() {
    noInterrupts();
    memcpy(&datosCamara, (const void*)&bufferCamara, sizeof(MensajeVision));
    interrupts();
}

// ---------------------------------------------------------------------------
// 4. INICIALIZACIÓN DE LA RADIO
// ---------------------------------------------------------------------------
void inicializarRadio() {
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ERROR: ESP-NOW no pudo inicializarse. Revisar antena/WiFi.");
        return;
    }

    esp_now_register_recv_cb(esp_now_recv_cb_t(alRecibirDatos));
    Serial.println("Radio ESP-NOW lista.");
}

// ---------------------------------------------------------------------------
// 5. FUNCIONES AUXILIARES
// ---------------------------------------------------------------------------
bool hayDatosNuevos() {
    return datosNuevosRecibidos;
}

void limpiarBanderaDatos() {
    datosNuevosRecibidos = false;
}