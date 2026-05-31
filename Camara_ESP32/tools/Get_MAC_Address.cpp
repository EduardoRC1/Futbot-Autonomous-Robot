#include <Arduino.h>
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
}

void loop() {
  // Imprime la MAC repetidamente para no perderla si el monitor
  // se conecta despues del arranque (problema comun con ESP32-CAM).
  Serial.println("\n-----------------------------------");
  Serial.print("LA DIRECCION MAC DE ESTA PLACA ES: ");
  Serial.println(WiFi.macAddress());
  Serial.println("-----------------------------------");
  delay(2000);
}
