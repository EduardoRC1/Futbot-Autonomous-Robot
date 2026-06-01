// Test para leer la direccion MAC del ESP32.
// Imprime la MAC cada 2 segundos en loop() para no perderla en el monitor
// (util para confirmar cual placa es cual y no confundir Robot A con Robot B).
//
// Uso: copia este archivo a src/ (deja solo UN .cpp con setup()/loop() en src/),
// Build -> Upload -> Monitor a 115200.
#include <Arduino.h>
#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_MODE_STA);
}

void loop() {
    Serial.println("\n-----------------------------------");
    Serial.print("LA DIRECCION MAC DE ESTA PLACA ES: ");
    Serial.println(WiFi.macAddress());
    Serial.println("-----------------------------------");
    delay(2000);
}
