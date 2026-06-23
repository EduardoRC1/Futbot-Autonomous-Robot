#include "Comunicacion.h"
#include "Config.h"
#include "DualSerial.h"
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

// Spinlock para proteger el buffer entre Core 0 (callback) y Core 1 (loop).
// noInterrupts() solo afecta al núcleo actual; portMUX bloquea ambos.
static portMUX_TYPE muxCamara = portMUX_INITIALIZER_UNLOCKED;

static volatile MensajeVision bufferCamara;

MensajeVision datosCamara;

static volatile bool          datosNuevosRecibidos = false;
static volatile unsigned long tiempoUltimoMensaje  = 0;
static volatile unsigned long contadorMensajes     = 0;

// Callback ESP-NOW — se ejecuta en Core 0
static void alRecibirDatos(const uint8_t* mac,
                           const uint8_t* datos, int len) {
    if (memcmp(mac, direccionMacCamara, 6) != 0) return;
    if (len != sizeof(MensajeVision)) return;

    portENTER_CRITICAL(&muxCamara);
    memcpy((void*)&bufferCamara, datos, sizeof(MensajeVision));
    datosNuevosRecibidos = true;
    tiempoUltimoMensaje  = millis();
    contadorMensajes++;
    portEXIT_CRITICAL(&muxCamara);
}

void inicializarRadio() {
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) {
        dualPrintln("[Radio] ERROR: ESP-NOW no pudo inicializarse");
        return;
    }

    esp_now_register_recv_cb(esp_now_recv_cb_t(alRecibirDatos));
    dualPrintln("[Radio] ESP-NOW lista");
}

void obtenerDatosCamara() {
    portENTER_CRITICAL(&muxCamara);
    memcpy(&datosCamara, (const void*)&bufferCamara, sizeof(MensajeVision));
    portEXIT_CRITICAL(&muxCamara);
}

void revisarConexionSegura() {
    if (millis() - tiempoUltimoMensaje > TIMEOUT_CAMARA_MS) {
        datosCamara.balonDetectado = false;
    }
}

bool hayDatosNuevos() { return datosNuevosRecibidos; }

void limpiarBanderaDatos() { datosNuevosRecibidos = false; }

bool camaraConectada() {
    return (millis() - tiempoUltimoMensaje) <= TIMEOUT_CAMARA_MS;
}

unsigned long obtenerContadorMensajes() { return contadorMensajes; }
