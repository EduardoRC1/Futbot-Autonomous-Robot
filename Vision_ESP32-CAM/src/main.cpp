#include "esp_camera.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>


// Aqui va el codigo MAC de la camara
uint8_t miMacAddress[] = {0x88, 0x57, 0x21, 0xC2, 0x0B, 0x68};

void setup() {
  Serial.begin(115200);

  // Configuramos WiFi en modo Estación para activar la MAC
  WiFi.mode(WIFI_STA);
  Serial.print("Iniciando Robot Defensivo con MAC: ");
  Serial.println(WiFi.macAddress());

  // Inicializar ESP-NOW (Para comunicación estratégica autónoma)
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error inicializando comunicación");
  }

  // ... (Aquí iría el resto de la configuración de la cámara y motores)
}

void loop() {
  // Aquí el robot ejecuta su lógica autónoma de defensa
  // 1. Leer sensores láser
  // 2. Procesar imagen de cámara
  // 3. Mover motores N20
}