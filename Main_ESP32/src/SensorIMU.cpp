#include "SensorIMU.h"
#include "Config.h"
#include "DualSerial.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

static Adafruit_BNO055 bno(55, BNO055_DIRECCION_I2C, &Wire);
static bool imuActiva = false;
static float offsetBrujula = 0.0f;

bool inicializarIMU() {
    dualPrintln("[IMU] Iniciando BNO055...");

    for (int intento = 1; intento <= 3; intento++) {
        if (bno.begin(OPERATION_MODE_NDOF)) {
            imuActiva = true;
            bno.setExtCrystalUse(true);
            // Esperar a que el sensor se estabilice antes de leer el offset
            delay(500);
            sensors_event_t evento;
            bno.getEvent(&evento, Adafruit_BNO055::VECTOR_EULER);
            offsetBrujula = evento.orientation.x;
            dualPrintf("[IMU] BNO055 OK en 0x%02X (intento %d)\n",
                       BNO055_DIRECCION_I2C, intento);
            dualPrintf("[IMU] Offset de brujula: %.1f° (esta direccion = 0°)\n",
                       offsetBrujula);
            return true;
        }
        dualPrintf("[IMU] BNO055 intento %d fallido\n", intento);
        delay(200);
    }

    dualPrintln("[IMU] ERROR: BNO055 no respondió después de 3 intentos");
    imuActiva = false;
    return false;
}

float leerRumboBrujula() {
    if (!imuActiva) return 0.0f;

    sensors_event_t evento;
    bno.getEvent(&evento, Adafruit_BNO055::VECTOR_EULER);
    float relativo = evento.orientation.x - offsetBrujula;
    if (relativo < 0.0f) relativo += 360.0f;
    if (relativo >= 360.0f) relativo -= 360.0f;
    return relativo;
}
