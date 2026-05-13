#include "SensorIMU.h"
#include "Config.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

static Adafruit_BNO055 bno(55, BNO055_DIRECCION_I2C, &Wire);
static bool imuActiva = false;

bool inicializarIMU() {
    Serial.println("[IMU] Iniciando BNO055...");

    for (int intento = 1; intento <= 3; intento++) {
        if (bno.begin(OPERATION_MODE_NDOF)) {
            imuActiva = true;
            bno.setExtCrystalUse(true);
            Serial.printf("[IMU] BNO055 OK en 0x%02X (intento %d)\n",
                          BNO055_DIRECCION_I2C, intento);
            return true;
        }
        Serial.printf("[IMU] BNO055 intento %d fallido\n", intento);
        delay(200);
    }

    Serial.println("[IMU] ERROR: BNO055 no respondió después de 3 intentos");
    imuActiva = false;
    return false;
}

float leerRumboBrujula() {
    if (!imuActiva) return 0.0f;

    sensors_event_t evento;
    bno.getEvent(&evento, Adafruit_BNO055::VECTOR_EULER);
    return evento.orientation.x;
}
