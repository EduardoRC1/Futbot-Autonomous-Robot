#include "esp_camera.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "ProtocoloEspNow.h" // <--- Usar el archivo centralizado

MensajeVision datosSalida;

// --- CALIBRACIÓN DE COLOR AMPLIA (Modo de Búsqueda) ---
int rMin = 90,  rMax = 255; // Permitimos rojos/naranjas más oscuros
int gMin = 20,  gMax = 180; // Permitimos más mezcla de amarillo/luz
int bMin = 0,   bMax = 90;  // Permitimos un poco de reflejo azulado de ventanas o monitores

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
  config.fb_count = 1;

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

  // OPTIMIZACIÓN: Saltamos píxeles de 4 en 4
  for (int i = 0; i < (fb->width * fb->height); i += 4) {
    
    // CORRECCIÓN 1: Leer los colores en el orden correcto (Extracción byte por byte)
    // Esto cura el "daltonismo" del ESP32
    uint8_t highByte = fb->buf[i * 2];
    uint8_t lowByte  = fb->buf[i * 2 + 1];
    
    // Extracción matemática exacta de RGB565 a escala 0-255
    uint8_t r = highByte & 0xF8;
    uint8_t g = ((highByte & 0x07) << 5) | ((lowByte & 0xE0) >> 3);
    uint8_t b = (lowByte & 0x1F) << 3;

    if (r >= rMin && r <= rMax && g >= gMin && g <= gMax && b >= bMin && b <= bMax) {
      sumaX += (i % fb->width);
      sumaY += (i / fb->width);
      pixelesEncontrados++;
    }
  }

  // CORRECCIÓN 2: Exigimos ver al menos 150 píxeles reales, no solo 15 de estática
  if (pixelesEncontrados > 50) { 
    datosSalida.balonDetectado = true;
    datosSalida.coordX = sumaX / pixelesEncontrados;
    datosSalida.coordY = sumaY / pixelesEncontrados;
    
    // Cálculo de distancia básica
    datosSalida.distanciaEstimada = 5000.0 / sqrt(pixelesEncontrados); 
  } else {
    datosSalida.balonDetectado = false;
  }

  // Enviar al Cerebro
  esp_now_send(direccionMacCerebro, (uint8_t *) &datosSalida, sizeof(datosSalida));
  
  esp_camera_fb_return(fb); 
}