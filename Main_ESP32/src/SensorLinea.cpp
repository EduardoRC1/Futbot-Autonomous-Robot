#include "SensorLinea.h"
#include "Config.h"

// TODO: Integrar QTRSensors cuando el hardware esté conectado.
// Incluir <QTRSensors.h> y configurar los pines analógicos aquí.

static bool lineaActiva = false;

void inicializarSensorLinea() {
    Serial.println("[Linea] QTR-8A pendiente de integración");
    lineaActiva = false;
}

bool detectarLineaBlanca() {
    if (!lineaActiva) return false;
    // Leer valores del arreglo QTR y comparar con umbral
    return false;
}
