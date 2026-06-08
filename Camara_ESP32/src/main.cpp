#include "esp_camera.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "ProtocoloEspNow.h"

MensajeVision datosSalida;

// --- CALIBRACION DE COLOR — Basketball naranja ---
// Ajustar estos valores si hay falsos positivos (detecta cosas que no son balón)
// o falsos negativos (no detecta el balón). El log de diagnóstico imprime
// los valores RGB promedio del centro del frame para ayudar a calibrar.
int rMin = 120, rMax = 255;
int gMin = 30,  gMax = 170;
int bMin = 0,   bMax = 80;
static const int UMBRAL_PIXELES_BALON = 100;

static bool camaraOK  = false;
static bool espnowOK  = false;
static unsigned long ultimoLog = 0;
static unsigned long frameCount = 0;

// Callback para saber si el envio ESP-NOW fue exitoso
static void onSend(const uint8_t *mac, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("[CAM] ERROR: envio ESP-NOW fallido");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("==========================================");
#ifdef ROBOT_A
  Serial.println("  FUTBOT CAMARA — Robot A");
#else
  Serial.println("  FUTBOT CAMARA — Robot B");
#endif
  Serial.println("==========================================");

  // Configuracion de Hardware (Ai-Thinker)
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
  if (err != ESP_OK) {
    Serial.printf("[CAM] ERROR: camara no inicializo (0x%x)\n", err);
    return;
  }
  camaraOK = true;
  Serial.println("[CAM] Camara inicializada OK (QVGA RGB565)");

  // Configuracion ESP-NOW
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  Serial.printf("[CAM] MAC propia: %s\n", WiFi.macAddress().c_str());
  Serial.printf("[CAM] MAC cerebro destino: %02X:%02X:%02X:%02X:%02X:%02X\n",
    direccionMacCerebro[0], direccionMacCerebro[1], direccionMacCerebro[2],
    direccionMacCerebro[3], direccionMacCerebro[4], direccionMacCerebro[5]);

  if (esp_now_init() != ESP_OK) {
    Serial.println("[CAM] ERROR: ESP-NOW no inicializo");
    return;
  }

  esp_now_register_send_cb(onSend);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, direccionMacCerebro, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("[CAM] ERROR: no se pudo agregar peer");
    return;
  }

  espnowOK = true;
  Serial.println("[CAM] ESP-NOW listo, enviando al cerebro");
  Serial.println("==========================================");
  Serial.println();
}

void loop() {
  if (!camaraOK || !espnowOK) {
    delay(1000);
    return;
  }

  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("[CAM] ERROR: no se obtuvo frame");
    return;
  }

  int pixelesEncontrados = 0;
  long sumaX = 0, sumaY = 0;

  // Saltamos pixeles de 4 en 4 para optimizar
  for (int i = 0; i < (fb->width * fb->height); i += 4) {

    uint8_t highByte = fb->buf[i * 2];
    uint8_t lowByte  = fb->buf[i * 2 + 1];

    // Extraccion RGB565 a escala 0-255
    uint8_t r = highByte & 0xF8;
    uint8_t g = ((highByte & 0x07) << 5) | ((lowByte & 0xE0) >> 3);
    uint8_t b = (lowByte & 0x1F) << 3;

    if (r >= rMin && r <= rMax && g >= gMin && g <= gMax && b >= bMin && b <= bMax) {
      sumaX += (i % fb->width);
      sumaY += (i / fb->width);
      pixelesEncontrados++;
    }
  }

  if (pixelesEncontrados > UMBRAL_PIXELES_BALON) {
    datosSalida.balonDetectado = true;
    datosSalida.coordX = sumaX / pixelesEncontrados;
    datosSalida.coordY = sumaY / pixelesEncontrados;
    datosSalida.distanciaEstimada = 5000.0 / sqrt(pixelesEncontrados);
  } else {
    datosSalida.balonDetectado = false;
  }

  // --- Deteccion de porteria enemiga ---
  // Buscar pixeles blancos/brillantes en la zona central-superior del frame.
  // La porteria se ve como una franja clara (blanca/gris) en la mitad superior.
  int porteriaPixeles = 0;
  int mitadAltura = fb->height / 2;
  int tercioIzq   = fb->width / 3;
  int tercioDer   = fb->width * 2 / 3;

  for (int y = 0; y < mitadAltura; y += 4) {
    for (int x = tercioIzq; x < tercioDer; x += 4) {
      int idx = y * fb->width + x;
      uint8_t hi = fb->buf[idx * 2];
      uint8_t lo = fb->buf[idx * 2 + 1];
      uint8_t r = hi & 0xF8;
      uint8_t g = ((hi & 0x07) << 5) | ((lo & 0xE0) >> 3);
      uint8_t b = (lo & 0x1F) << 3;
      if (r > 180 && g > 180 && b > 180) {
        porteriaPixeles++;
      }
    }
  }
  datosSalida.porteriaEnemigaAlineada = (porteriaPixeles > 80);

  // Enviar al Cerebro
  esp_now_send(direccionMacCerebro, (uint8_t *) &datosSalida, sizeof(datosSalida));

  // --- Muestreo RGB del centro del frame para diagnóstico de calibración ---
  // Promedia un bloque de 8x8 píxeles en el centro de la imagen.
  long sumR = 0, sumG = 0, sumB = 0;
  int centroX = fb->width / 2;
  int centroY = fb->height / 2;
  int muestras = 0;
  for (int dy = -4; dy < 4; dy++) {
    for (int dx = -4; dx < 4; dx++) {
      int idx = (centroY + dy) * fb->width + (centroX + dx);
      uint8_t hi = fb->buf[idx * 2];
      uint8_t lo = fb->buf[idx * 2 + 1];
      sumR += (hi & 0xF8);
      sumG += ((hi & 0x07) << 5) | ((lo & 0xE0) >> 3);
      sumB += (lo & 0x1F) << 3;
      muestras++;
    }
  }
  uint8_t avgR = sumR / muestras;
  uint8_t avgG = sumG / muestras;
  uint8_t avgB = sumB / muestras;

  esp_camera_fb_return(fb);
  frameCount++;

  // Log de diagnostico cada 3 segundos
  if (millis() - ultimoLog > 3000) {
    ultimoLog = millis();
    if (datosSalida.balonDetectado) {
      Serial.printf("[CAM] BALON SI — x=%d y=%d dist=%.1f px=%d Port=%s RGB=%d,%d,%d frames=%lu\n",
        datosSalida.coordX, datosSalida.coordY,
        datosSalida.distanciaEstimada, pixelesEncontrados,
        datosSalida.porteriaEnemigaAlineada ? "SI" : "no",
        avgR, avgG, avgB, frameCount);
    } else {
      Serial.printf("[CAM] Balon no — px=%d Port=%s RGB=%d,%d,%d frames=%lu\n",
        pixelesEncontrados,
        datosSalida.porteriaEnemigaAlineada ? "SI" : "no",
        avgR, avgG, avgB, frameCount);
    }
  }
}
