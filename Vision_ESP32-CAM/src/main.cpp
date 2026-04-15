#define ROBOT_A // Descomentar para compilar el código del Robot A (Cerebro A)
// #define ROBOT_B // Descomentar para compilar el código del Robot B (Cerebro B)


#include "esp_camera.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "ProtocoloEspNow.h" // <--- Usar el archivo centralizado

MensajeVision datosSalida;

// --- CALIBRACIÓN DE COLOR (Ajustar en el torneo) ---
int rMin = 150, rMax = 255;
int gMin = 50,  gMax = 150;
int bMin = 0,   bMax = 60;

void setup() {
  Serial.begin(115200);

  // Configuración de Hardware (Ai-Thinker)
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5; config.pin_d1 = 18; config.pin_d2 = 19; config.pin_d3 = 21;
  config.pin_d4 = 36; config.pin_d5 = 39; config.pin_d6 = 34; config.pin_d7 = 35;
  config.pin_xclk = 0; config.pin_pclk = 22; config.pin_vsync = 25; config.pin_href = 23;
  config.pin_sscb_sda = 26; config.pin_sscb_scl = 27; config.pin_pwdn = 32;
  config.pin_reset = -1; config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565; 
  config.frame_size = FRAMESIZE_QVGA;     
  config.fb_count = 2; // <--- Cambiado a 2 para procesar un frame mientras se captura otro (Double Buffer)

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) return;

  // Configuración ESP-NOW
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) return;

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, direccionMacCerebro, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
}

void loop() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) return;

  int pixelesEncontrados = 0;
  long sumaX = 0, sumaY = 0;

  // OPTIMIZACIÓN: Saltamos píxeles de 4 en 4 (Paso de 8 bytes en RGB565)
  // Esto aumenta los FPS significativamente sin perder mucha precisión.
  for (int i = 0; i < (fb->width * fb->height); i += 4) {
    uint16_t pixel = ((uint16_t*)fb->buf)[i];
    
    // Extracción rápida de RGB
    uint8_t r = (pixel >> 11) << 3;
    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
    uint8_t b = (pixel & 0x1F) << 3;

    if (r >= rMin && r <= rMax && g >= gMin && g <= gMax && b >= bMin && b <= bMax) {
      sumaX += (i % fb->width);
      sumaY += (i / fb->width);
      pixelesEncontrados++;
    }
  }

  // Solo enviamos si la "masa" de color es suficiente para no perseguir ruido
  if (pixelesEncontrados > 15) { 
    datosSalida.balonDetectado = true;
    datosSalida.coordX = sumaX / pixelesEncontrados;
    datosSalida.coordY = sumaY / pixelesEncontrados;
    
    // Cálculo de distancia básica (entre más píxeles, más cerca está)
    datosSalida.distanciaEstimada = 5000.0 / sqrt(pixelesEncontrados); 
  } else {
    datosSalida.balonDetectado = false;
  }

  // Enviar al Cerebro
  esp_now_send(direccionMacCerebro, (uint8_t *) &datosSalida, sizeof(datosSalida));
  
  esp_camera_fb_return(fb); 
}