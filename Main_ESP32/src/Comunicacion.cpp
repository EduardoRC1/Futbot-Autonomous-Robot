// Comunicacion.cpp
#include <Arduino.h> 
#include "Comunicacion.h"
#include "ProtocoloEspNow.h"
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

MensajeVision datosCamara;
bool datosNuevosRecibidos = false;

// Variable de seguridad contra "fantasmas"
unsigned long tiempoUltimoMensaje = 0; 

// 1. FUNCIÓN QUE SE EJECUTA AL RECIBIR DATOS
void alRecibirDatos(const uint8_t * mac, const uint8_t *datosEntrantes, int len) {
  // Verificamos que el mensaje venga de la MAC de nuestra cámara
  if (memcmp(mac, direccionMacCamara, 6) == 0) {
    
    // Copiamos los datos recibidos a nuestra variable global
    memcpy(&datosCamara, datosEntrantes, sizeof(datosCamara));
    
    // ¡CRÍTICO! Levantamos la bandera para que la Estrategia sepa que hay datos
    datosNuevosRecibidos = true; 
    
    // Registramos la hora exacta
    tiempoUltimoMensaje = millis(); 
  } 
}

// 2. FUNCIÓN DE SEGURIDAD (BOTÓN DE PÁNICO)
void revisarConexionSegura() {
    // Si ha pasado mas de 500 milisegundos sin un mensaje...
    if (millis() - tiempoUltimoMensaje > 500) {
        datosCamara.balonDetectado = false; // Forzamos a que deje de ver fantasmas
    }
}

// 3. INICIALIZACIÓN DE LA RADIO
void inicializarRadio() {
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); // Canal mágico

    if (esp_now_init() != ESP_OK) {
          Serial.println("Error iniciando ESP-NOW");
        return;
    }

    // ¡CRÍTICO! Aquí conectamos la radio con nuestra función
    // Esto le dice al ESP32: "Cada vez que recibas algo, ejecuta alRecibirDatos"
    esp_now_register_recv_cb(esp_now_recv_cb_t(alRecibirDatos)); 
}

// 4. FUNCIONES AUXILIARES
bool hayDatosNuevos() {
    return datosNuevosRecibidos; 
}

void limpiarBanderaDatos() {
    datosNuevosRecibidos = false;
}
