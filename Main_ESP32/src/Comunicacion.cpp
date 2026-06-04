#include "Comunicacion.h"
#include "Config.h"
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

static volatile MensajeVision bufferCamara;

MensajeVision datosCamara;

static volatile bool          datosNuevosRecibidos = false;
static volatile unsigned long tiempoUltimoMensaje  = 0;

// Callback ESP-NOW — se ejecuta en Core 0
static void alRecibirDatos(const uint8_t* mac,
                           const uint8_t* datos, int len) {
    if (memcmp(mac, direccionMacCamara, 6) != 0) return;
    if (len != sizeof(MensajeVision)) return;

    memcpy((void*)&bufferCamara, datos, sizeof(MensajeVision));
    datosNuevosRecibidos = true;
    tiempoUltimoMensaje  = millis();
}

void inicializarRadio() {
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) {
        Serial.println("[Radio] ERROR: ESP-NOW no pudo inicializarse");
        return;
    }

    esp_now_register_recv_cb(esp_now_recv_cb_t(alRecibirDatos));
    Serial.println("[Radio] ESP-NOW lista");
}

void obtenerDatosCamara() {
    noInterrupts();
    memcpy(&datosCamara, (const void*)&bufferCamara, sizeof(MensajeVision));
    interrupts();
}

void revisarConexionSegura() {
    if (millis() - tiempoUltimoMensaje > TIMEOUT_CAMARA_MS) {
        datosCamara.balonDetectado = false;
    }
}

bool hayDatosNuevos() { return datosNuevosRecibidos; }

void limpiarBanderaDatos() { datosNuevosRecibidos = false; }
