// Comunicacion.cpp, aqui se implementara la logica de comunicacion entre el robot y la camara usando
// ESP-NOW, un protocolo de comunicacion inalámbrica de baja latencia
#include "Comunicacion.h"
#include "ProtocoloEspNow.h"
#include <WiFi.h> // Necesario para usar ESP-NOW, aunque no estemos usando WiFi tradicional
#include <esp_now.h> // Libreria de ESP-NOW para manejar la comunicacion entre el robot y la camara

MensajeVision datosCamara;
bool datosNuevosRecibidos = false;

// Esta función se llama automáticamente cada vez que se reciben datos a través de ESP-NOW
void alRecibirDatos(const uint8_t * mac, const uint8_t *datosEntrantes, int len) {
  
  // Filtro de Seguridad: Comparamos la MAC entrante con la de nuestra camara
  if (memcmp(mac, direccionMacCamara, 6) == 0) {
    // Si coinciden, copiamos los datos y levantamos la bandera
    memcpy(&datosCamara, datosEntrantes, sizeof(datosCamara));
    datosNuevosRecibidos = true;
  } 
  // Si no coinciden, el codigo simplemente ignora el mensaje (era de otro robot)
}

// Función para inicializar la comunicación ESP-NOW
void inicializarRadio() {
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) return;
    esp_now_register_recv_cb(alRecibirDatos);
}

bool hayDatosNuevos() {return datosNuevosRecibidos; }
void limpiarBanderaDatos() {datosNuevosRecibidos = false;}
