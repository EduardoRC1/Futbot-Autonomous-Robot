#include "Comunicacion.h"
#include <WiFi.h>
#include <esp_now.h>

MensajeVision datosCamara;
bool datosNuevosRecibidos = false;

// Esta función se llama automáticamente cada vez que se reciben datos a través de ESP-NOW
void alRecibirDatos(const uint8_t * mac, const uint8_t *datosEntrantes, int len) {
    memcpy(&datosCamara, datosEntrantes, sizeof(datosCamara));
    datosNuevosRecibidos = true;
}

// Función para inicializar la comunicación ESP-NOW
void inicializarRadio() {
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error al inicializar ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(alRecibirDatos);
    Serial.println("Radio ESP-NOW lista.");
}

// Función para verificar si hay nuevos datos recibidos
bool hayDatosNuevos() {
    return datosNuevosRecibidos;
}

// Función para limpiar la bandera de datos nuevos después de procesarlos
void limpiarBanderaDatos() {
    datosNuevosRecibidos = false;
}
