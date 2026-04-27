#include "Sensores.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_VL53L0X.h>
#include <QTRSensors.h>

const int TOF_FRONT_XSHUT = 5; 
const int TOF_LEFT_XSHUT = 18; 
const int TOF_RIGHT_XSHUT = 19; 

Adafruit_BNO055 brujula = Adafruit_BNO055(55, 0x28, &Wire);
Adafruit_VL53L0X tofFrente = Adafruit_VL53L0X(); 
Adafruit_VL53L0X tofIzquierda = Adafruit_VL53L0X();
Adafruit_VL53L0X tofDerecha = Adafruit_VL53L0X();
QTRSensors qtr; 

// SOLO 6 PINES SEGUROS (ADC1) PARA EVITAR QUE EL WIFI CRASHEE EL ESP32
const uint8_t SensorPin[6] = {34, 35, 36, 39, 32, 33}; 
uint16_t valoresSensor[6];

void inicializarBusI2C() { Wire.begin(); }

void inicializarIMU_BNO055() {
    if (brujula.begin()) { brujula.setExtCrystalUse(true); }
}

void inicializarToF_VL53L0X() {
    pinMode(TOF_FRONT_XSHUT, OUTPUT); pinMode(TOF_LEFT_XSHUT, OUTPUT); pinMode(TOF_RIGHT_XSHUT, OUTPUT);
    // Apagar todos
    digitalWrite(TOF_FRONT_XSHUT, LOW); digitalWrite(TOF_LEFT_XSHUT, LOW); digitalWrite(TOF_RIGHT_XSHUT, LOW);
    delay(10);
    
    // Encender Frente y cambiar direccion a 0x30
    digitalWrite(TOF_FRONT_XSHUT, HIGH); delay(10);
    tofFrente.begin(0x30); 
    
    // Encender Izquierda y cambiar direccion a 0x31
    digitalWrite(TOF_LEFT_XSHUT, HIGH); delay(10);
    tofIzquierda.begin(0x31);
    
    // Encender Derecha y mantener direccion default 0x29
    digitalWrite(TOF_RIGHT_XSHUT, HIGH); delay(10);
    tofDerecha.begin(0x29);
}

void inicializarLinea_QTR8A() {
    qtr.setTypeAnalog();
    qtr.setSensorPins(SensorPin, 6); // Configurado para 6 sensores
}

float leerRumboBrujula() {
    sensors_event_t event; brujula.getEvent(&event); return event.orientation.x; 
}

bool detectarOponenteFrente() {
    VL53L0X_RangingMeasurementData_t medicion; tofFrente.rangingTest(&medicion, false);
    if (medicion.RangeStatus != 4 && medicion.RangeMilliMeter < 200) return true;
    return false;
}

bool detectarLineaBlanca() {
    qtr.read(valoresSensor);
    for (uint8_t i = 0; i < 6; i++) { // Revisar los 6 sensores
        if (valoresSensor[i] < 200) return true; 
    }
    return false;
}
