#include "esp_camera.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

// =========================================================================
// 1. EL INTERRUPTOR DE ROBOTS (Cambia esto según la cámara que programes)
// =========================================================================
#define ROBOT_A  
// #define ROBOT_B 

#ifdef ROBOT_A
  uint8_t direccionMacCerebro[] = {0x00, 0x70, 0x07, 0x1C, 0x0F, 0xB0}; // Cerebro A
#elif defined(ROBOT_B)
  uint8_t direccionMacCerebro[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Cerebro B (Falta descubrir)
#else
  #error "Define ROBOT_A o ROBOT_B"
#endif

// =========================================================================
// 2. LA ESTRUCTURA DE DATOS (Debe ser IDÉNTICA a la del Cerebro Central)
// =========================================================================
typedef struct MensajeVision {
    bool balonDetectado;
    int16_t coordX;
    int16_t coordY;
    float distanciaEstimada;
    bool porteriaEnemigaAlineada;
} MensajeVision;

MensajeVision datosSalida;

// =========================================================================
// 3. CALIBRACIÓN DE COLOR (Valores para la pelota del torneo)
// =========================================================================
// Estos son valores de ejemplo para un naranja brillante. 
// Deberás ajustarlos en la cancha real usando el WebServer.
int rMin = 150, rMax = 255;
int gMin = 50,  gMax = 150;
int bMin = 0,   bMax = 60;

void setup() {
  Serial.begin(115200);

  // --- CONFIGURACIÓN DE LA CÁMARA (Modelo Ai-Thinker) ---
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5; config.pin_d1 = 18; config.pin_d2 = 19; config.pin_d3 = 21;
  config.pin_d4 = 36; config.pin_d5 = 39; config.pin_d6 = 34; config.pin_d7 = 35;
  config.pin_xclk = 0; config.pin_pclk = 22; config.pin_vsync = 25; config.pin_href = 23;
  config.pin_sscb_sda = 26; config.pin_sscb_scl = 27; config.pin_pwdn = 32;
  config.pin_reset = -1; config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_RGB565; // Formato a color para procesar rápido
  config.frame_size = FRAMESIZE_QVGA;     // Resolución baja = procesador más rápido
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Error fatal: Cámara no iniciada 0x%x", err);
    return;
  }

  // --- CONFIGURACIÓN ESP-NOW ---
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) return;

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, direccionMacCerebro, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.println("Cámara Lista. Iniciando visión...");
}

void loop() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) return;

  int pixelesEncontrados = 0;
  long sumaX = 0, sumaY = 0;

  // Escaneo rápido de la imagen (saltando píxeles de 2 en 2 para mayor velocidad)
  for (int i = 0; i < (fb->width * fb->height); i += 2) {
    uint16_t pixel = ((uint16_t*)fb->buf)[i];
    uint8_t r = (pixel >> 11) << 3;
    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
    uint8_t b = (pixel & 0x1F) << 3;

    // Si el píxel está dentro del rango de color de nuestra pelota...
    if (r >= rMin && r <= rMax && g >= gMin && g <= gMax && b >= bMin && b <= bMax) {
      sumaX += (i % fb->width);
      sumaY += (i / fb->width);
      pixelesEncontrados++;
    }
  }

  // Si encontramos una mancha de color lo suficientemente grande...
  if (pixelesEncontrados > 20) { 
    datosSalida.balonDetectado = true;
    datosSalida.coordX = sumaX / pixelesEncontrados; // Centroide X
    datosSalida.coordY = sumaY / pixelesEncontrados; // Centroide Y
    Serial.printf("Pelota en: X=%d, Y=%d\n", datosSalida.coordX, datosSalida.coordY);
  } else {
    datosSalida.balonDetectado = false;
  }

  // Enviar paquete al Cerebro Central
  esp_now_send(direccionMacCerebro, (uint8_t *) &datosSalida, sizeof(datosSalida));
  
  esp_camera_fb_return(fb); // Liberar memoria (Crítico para que no se congele)
}