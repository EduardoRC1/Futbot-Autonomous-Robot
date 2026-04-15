// Comunicacion.cpp, aqui se implementara la logica de comunicacion entre el robot y la camara usando
// ESP-NOW, un protocolo de comunicacion inalámbrica de baja latencia
#include "Comunicacion.h"
#include "ProtocoloEspNow.h"
#include <WiFi.h> // Necesario para usar ESP-NOW, aunque no estemos usando WiFi tradicional
#include <esp_now.h> // Libreria de ESP-NOW para manejar la comunicacion entre el robot y la camara
#include <esp_wifi.h> // Para configurar el modo WiFi y asegurarnos de que ESP-NOW funcione correctamente

MensajeVision datosCamara;
bool datosNuevosRecibidos = false;

// Esta función se llama automáticamente cada vez que se reciben datos a través de ESP-NOW
// Esta función se llama automáticamente cada vez que se reciben datos a través de ESP-NOW
void alRecibirDatos(const uint8_t * mac, const uint8_t *datosEntrantes, int len) {
  
  // Filtro de Seguridad: Comparamos la MAC entrante con la de nuestra camara
  if (memcmp(mac, direccionMacCamara, 6) == 0) {
    // Si coinciden, copiamos los datos y levantamos la bandera
    memcpy(&datosCamara, datosEntrantes, sizeof(datosCamara));
    datosNuevosRecibidos = true;

    // --- ¡LA PIEZA FALTANTE: IMPRIMIR EN PANTALLA! ---
    if (datosCamara.balonDetectado) {
        Serial.printf("¡PELOTA RECIBIDA POR RADIO! X: %d, Y: %d\n", datosCamara.coordX, datosCamara.coordY);
    }
  } 
}
unsigned long tiempoUltimoMensaje = 0; // Agrega esto hasta arriba

void alRecibirDatos(const uint8_t * mac, const uint8_t *datosEntrantes, int len) {
  if (memcmp(mac, direccionMacCamara, 6) == 0) {
    memcpy(&datosCamara, datosEntrantes, sizeof(datosCamara));
    tiempoUltimoMensaje = millis(); // <--- Registramos la hora exacta del mensaje
  } 
}

// Agrega esta nueva función al final del archivo
void revisarConexionSegura() {
    // Si ha pasado mas de 500 milisegundos sin un mensaje...
    if (millis() - tiempoUltimoMensaje > 500) {
        datosCamara.balonDetectado = false; // Forzamos a que deje de ver fantasmas
    }
}

// Función para inicializar la comunicación ESP-NOW
void inicializarRadio() {
   WiFi.mode(WIFI_STA);
esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); // <--- LA LÍNEA MÁGICA PARA EL CEREBRO

if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
}
}

bool hayDatosNuevos() {return datosNuevosRecibidos; }
void limpiarBanderaDatos() {datosNuevosRecibidos = false;}
