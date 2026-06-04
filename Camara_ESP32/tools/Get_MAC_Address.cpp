#include <Arduino.h>
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);

  Serial.println("\n-----------------------------------");
  Serial.print("LA DIRECCION MAC DE ESTA PLACA ES: ");
  Serial.println(WiFi.macAddress());
  Serial.println("-----------------------------------\n");
}

void loop() {
  delay(1000);
}
