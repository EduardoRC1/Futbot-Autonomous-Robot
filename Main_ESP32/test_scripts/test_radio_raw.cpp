#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// Debe ser exactamente igual a la estructura de tu cámara
typedef struct MensajeVision {
    bool balonDetectado;
    int coordX;
    int coordY;
    float distanciaEstimada;
} MensajeVision;

MensajeVision datosCamara;

void alRecibirDatos(const uint8_t * mac, const uint8_t *datosRecibidos, int longitud) {
    memcpy(&datosCamara, datosRecibidos, sizeof(datosCamara));
    
    Serial.print("RADIO OK -> Pelota: ");
    Serial.print(datosCamara.balonDetectado);
    Serial.print(" | X: ");
    Serial.print(datosCamara.coordX);
    Serial.print(" | Dist: ");
    Serial.println(datosCamara.distanciaEstimada);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error iniciando ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(alRecibirDatos);
    Serial.println("Modo de escucha de radio RAW activado...");
}

void loop() {
    // No hace nada más que escuchar
    delay(1000);
}