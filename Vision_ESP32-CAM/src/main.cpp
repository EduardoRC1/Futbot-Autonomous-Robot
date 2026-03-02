#include <Arduino.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <esp_now.h>
#include "ProtocoloEspNow.h"

// AI Thinker ESP32-CAM Pines
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

MensajeVision datosSalientes;
esp_now_peer_info_t infoCerebroCentral;

void alEnviarDatos(const uint8_t *mac_addr, esp_now_send_status_t status) {}

void setup() {
    Serial.begin(115200);

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_RGB565; 
    config.frame_size = FRAMESIZE_QVGA;     
    config.jpeg_quality = 12;
    config.fb_count = 1;

    if (esp_camera_init(&config) != ESP_OK) return;

    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) return;
    
    esp_now_register_send_cb(alEnviarDatos);
    
    memcpy(infoCerebroCentral.peer_addr, direccionMacCerebro, 6);
    infoCerebroCentral.channel = 0;  
    infoCerebroCentral.encrypt = false;
    esp_now_add_peer(&infoCerebroCentral);
}

void loop() {
    camera_fb_t * frame = esp_camera_fb_get();
    if (!frame) return;

    // EL EQUIPO PROGRAMARÁ LA VISIÓN ARTIFICIAL AQUÍ

    // Datos de prueba para simular que vemos el balón
    datosSalientes.balonDetectado = true;
    datosSalientes.coordX = 160; 
    datosSalientes.coordY = 120;
    datosSalientes.distanciaEstimada = 45.0; 
    datosSalientes.porteriaEnemigaAlineada = false;

    esp_now_send(direccionMacCerebro, (uint8_t *) &datosSalientes, sizeof(datosSalientes));
    esp_camera_fb_return(frame);
    delay(100); 
}