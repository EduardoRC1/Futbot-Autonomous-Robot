#include "esp_camera.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <string.h>
#include <math.h>
#include "ProtocoloEspNow.h"

MensajeVision datosSalida;

// --- CALIBRACION DE COLOR — Basketball naranja ---
// Ajustar estos valores si hay falsos positivos (detecta cosas que no son balón)
// o falsos negativos (no detecta el balón). El log de diagnóstico imprime
// los valores RGB promedio del centro del frame para ayudar a calibrar.
int rMin = 130, rMax = 255;
int gMin = 40,  gMax = 130;
int bMin = 0,   bMax = 60;

// Gate anti-falsos-positivos: el naranja real tiene R dominante sobre G y muy
// por encima de B. Los grises/blancos/cafés tienen R~=G~=B y NO pasan esto,
// aunque caigan dentro de la caja RGB de arriba.
int NARANJA_R_SOBRE_G = 20;  // r debe superar a g por al menos esto
int NARANJA_R_SOBRE_B = 60;  // r debe superar a b por al menos esto

static inline bool esColorBalon(uint8_t r, uint8_t g, uint8_t b) {
  return r >= rMin && r <= rMax && g >= gMin && g <= gMax && b >= bMin && b <= bMax &&
         (int)r - (int)g >= NARANJA_R_SOBRE_G &&
         (int)r - (int)b >= NARANJA_R_SOBRE_B;
}

// ===========================================================================
//  RESOLUCIÓN DE LA CÁMARA  (CAMBIAR AQUÍ)
//  - VGA (640x480) ve MÁS LEJOS (4x más píxeles sobre un balón lejano), pero
//    baja los FPS y usa más memoria (requiere PSRAM, la AI-Thinker la tiene).
//  - QVGA (320x240) es más rápida. Si notas el robot lento/laggy, vuelve a QVGA
//    comentando la línea USAR_VGA.
//  IMPORTANTE: si cambias esto, actualiza CAM_CENTRO_X en Config.h del cerebro
//  (VGA -> 320, QVGA -> 160), porque la estrategia centra el balón con ese valor.
// ===========================================================================
#define USAR_VGA   // <- comenta esta línea para volver a QVGA

#ifdef USAR_VGA
  #define FRAME_SIZE_CAM            FRAMESIZE_VGA
  static const float CONST_DISTANCIA_BLOB = 8000.0f; // calibrar en cancha
  static const float TAM_BLOB_MIN_PX      = 5.0f;
#else
  #define FRAME_SIZE_CAM            FRAMESIZE_QVGA
  static const float CONST_DISTANCIA_BLOB = 4000.0f; // calibrar en cancha
  static const float TAM_BLOB_MIN_PX      = 4.0f;
#endif

// Muestreo: procesamos 1 de cada PASO_MUESTREO píxeles para optimizar.
static const int PASO_MUESTREO = 2;

// Mínimo de muestras de color "balón" para confirmar detección. Controla el
// ALCANCE junto con la resolución (NO los rangos de color rMin/gMin/...).
// Súbelo si hay falsos positivos; bájalo para ver más lejos.
static const int UMBRAL_PIXELES_BALON = 10;

// Rejilla para localizar el balón y RECHAZAR ruido disperso (píxeles naranjas
// sueltos lejos del balón). El balón es el grupo más denso de la rejilla.
static const int REJILLA_COLS = 16;
static const int REJILLA_FILS = 12;
static const int REJILLA_VENTANA = 3; // celdas alrededor del pico que se aceptan

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
  config.frame_size = FRAME_SIZE_CAM;
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

  const int W = fb->width;
  const int H = fb->height;

  // --- PASO 1: histograma grueso de muestras naranjas en una rejilla ---
  // El balón es el grupo MÁS DENSO; el ruido son píxeles sueltos repartidos.
  int hist[REJILLA_FILS][REJILLA_COLS];
  memset(hist, 0, sizeof(hist));

  for (int i = 0; i < (W * H); i += PASO_MUESTREO) {
    uint8_t highByte = fb->buf[i * 2];
    uint8_t lowByte  = fb->buf[i * 2 + 1];
    uint8_t r = highByte & 0xF8;
    uint8_t g = ((highByte & 0x07) << 5) | ((lowByte & 0xE0) >> 3);
    uint8_t b = (lowByte & 0x1F) << 3;
    if (esColorBalon(r, g, b)) {
      int cc = (i % W) * REJILLA_COLS / W;
      int cr = (i / W) * REJILLA_FILS / H;
      hist[cr][cc]++;
    }
  }

  // Celda con más muestras = centro aproximado del balón
  int pico = 0, picoFil = 0, picoCol = 0;
  for (int cr = 0; cr < REJILLA_FILS; cr++)
    for (int cc = 0; cc < REJILLA_COLS; cc++)
      if (hist[cr][cc] > pico) { pico = hist[cr][cc]; picoFil = cr; picoCol = cc; }

  int pixelesEncontrados = 0;
  long sumaX = 0, sumaY = 0;
  long long sumaX2 = 0, sumaY2 = 0;
  long blobR = 0, blobG = 0, blobB = 0;
  float tamanoBlob = 0.0f;

  if (pico >= 2) {
    // Solo aceptamos celdas densas y cercanas al pico (rechaza ruido lejano)
    int umbralCelda = pico / 4; if (umbralCelda < 2) umbralCelda = 2;

    // --- PASO 2: centroide y dispersión SOLO en la zona del balón ---
    for (int i = 0; i < (W * H); i += PASO_MUESTREO) {
      uint8_t highByte = fb->buf[i * 2];
      uint8_t lowByte  = fb->buf[i * 2 + 1];
      uint8_t r = highByte & 0xF8;
      uint8_t g = ((highByte & 0x07) << 5) | ((lowByte & 0xE0) >> 3);
      uint8_t b = (lowByte & 0x1F) << 3;
      if (esColorBalon(r, g, b)) {
        int x = i % W, y = i / W;
        int cc = x * REJILLA_COLS / W;
        int cr = y * REJILLA_FILS / H;
        int dcc = cc - picoCol; if (dcc < 0) dcc = -dcc;
        int dcr = cr - picoFil; if (dcr < 0) dcr = -dcr;
        if (dcc <= REJILLA_VENTANA && dcr <= REJILLA_VENTANA &&
            hist[cr][cc] >= umbralCelda) {
          sumaX += x; sumaY += y;
          sumaX2 += (long long)x * x;
          sumaY2 += (long long)y * y;
          blobR += r; blobG += g; blobB += b;
          pixelesEncontrados++;
        }
      }
    }

    if (pixelesEncontrados > 0) {
      float cx = (float)sumaX / pixelesEncontrados;
      float cy = (float)sumaY / pixelesEncontrados;
      float varX = (float)sumaX2 / pixelesEncontrados - cx * cx;
      float varY = (float)sumaY2 / pixelesEncontrados - cy * cy;
      if (varX < 0) varX = 0;
      if (varY < 0) varY = 0;
      // Para un disco lleno, diámetro aparente ~= 4*desviación estándar.
      tamanoBlob = 2.0f * (sqrtf(varX) + sqrtf(varY));
    }
  }

  // Detección: suficientes muestras Y un blob de tamaño mínimo coherente.
  if (pixelesEncontrados > UMBRAL_PIXELES_BALON && tamanoBlob > TAM_BLOB_MIN_PX) {
    datosSalida.balonDetectado = true;
    datosSalida.coordX = sumaX / pixelesEncontrados;
    datosSalida.coordY = sumaY / pixelesEncontrados;
    // Distancia por TAMAÑO APARENTE (geometría), NO por conteo de píxeles:
    // valor menor = balón más cerca. Calibrar CONST_DISTANCIA_BLOB en cancha.
    datosSalida.distanciaEstimada = CONST_DISTANCIA_BLOB / tamanoBlob;
  } else {
    datosSalida.balonDetectado = false;
  }

  // Color promedio REAL del blob detectado (lo que disparó la detección).
  // Sirve para calibrar: si en un falso positivo no es naranja, ajustar rangos.
  uint8_t blobAvgR = pixelesEncontrados ? blobR / pixelesEncontrados : 0;
  uint8_t blobAvgG = pixelesEncontrados ? blobG / pixelesEncontrados : 0;
  uint8_t blobAvgB = pixelesEncontrados ? blobB / pixelesEncontrados : 0;

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
      Serial.printf("[CAM] BALON SI — x=%d y=%d dist=%.1f blob=%.1f px=%d Port=%s blobRGB=%d,%d,%d frames=%lu\n",
        datosSalida.coordX, datosSalida.coordY,
        datosSalida.distanciaEstimada, tamanoBlob, pixelesEncontrados,
        datosSalida.porteriaEnemigaAlineada ? "SI" : "no",
        blobAvgR, blobAvgG, blobAvgB, frameCount);
    } else {
      Serial.printf("[CAM] Balon no — px=%d blob=%.1f Port=%s RGB=%d,%d,%d frames=%lu\n",
        pixelesEncontrados, tamanoBlob,
        datosSalida.porteriaEnemigaAlineada ? "SI" : "no",
        avgR, avgG, avgB, frameCount);
    }
  }
}
