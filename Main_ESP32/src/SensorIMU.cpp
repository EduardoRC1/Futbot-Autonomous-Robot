#include "SensorIMU.h"
#include "Config.h"

// TODO: Integrar Adafruit_BNO055 cuando el hardware esté conectado.
// Incluir <Adafruit_BNO055.h> y crear instancia aquí.

static bool imuActiva = false;

bool inicializarIMU() {
    Serial.println("[IMU] BNO055 pendiente de integración");
    imuActiva = false;
    return false;
}

float leerRumboBrujula() {
    if (!imuActiva) return 0.0f;
    // Leer sensor_event de tipo VECTOR_EULER y devolver event.orientation.x
    return 0.0f;
}
