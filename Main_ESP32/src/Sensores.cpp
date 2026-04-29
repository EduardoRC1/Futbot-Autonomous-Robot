#include "Sensores.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_VL53L0X.h>
#include <QTRSensors.h>

// Pines de control para el encendido secuencial (XSHUT)
const int TOF_FRONT_XSHUT = 5; 
const int TOF_LEFT_XSHUT = 18; 
const int TOF_RIGHT_XSHUT = 19; 

// CONFIGURACIÓN DE DIRECCIONES I2C:
// 0x29 -> Reservado para la Brújula (Según tu escáner)
// 0x30 -> Reservado para Láser Frente
// 0x31 -> Reservado para Láser Izquierda
// 0x32 -> Reservado para Láser Derecha

Adafruit_BNO055 brujula = Adafruit_BNO055(55, 0x28, &Wire);
Adafruit_VL53L0X tofFrente = Adafruit_VL53L0X(); 
Adafruit_VL53L0X tofIzquierda = Adafruit_VL53L0X();
Adafruit_VL53L0X tofDerecha = Adafruit_VL53L0X();
QTRSensors qtr; 

const uint8_t SensorPin[6] = {34, 35, 36, 39, 32, 33}; 
uint16_t valoresSensor[6];

void inicializarBusI2C() { Wire.begin(); }

void inicializarIMU_BNO055() {
    delay(500); 
    // IMPORTANTE: begin() va completamente vacío
    if (brujula.begin()) { 
        brujula.setExtCrystalUse(true); 
        Serial.println("BNO055 Iniciado OK en 0x28");
    } else {
        Serial.println("Error: BNO055 no encontrado en 0x28");
    }
}

void inicializarToF_VL53L0X() {
    pinMode(TOF_FRONT_XSHUT, OUTPUT); 
    pinMode(TOF_LEFT_XSHUT, OUTPUT); 
    pinMode(TOF_RIGHT_XSHUT, OUTPUT);

    // 1. Reset: Apagamos todos los láseres poniendo XSHUT en LOW
    digitalWrite(TOF_FRONT_XSHUT, LOW); 
    digitalWrite(TOF_LEFT_XSHUT, LOW); 
    digitalWrite(TOF_RIGHT_XSHUT, LOW);
    delay(10);
    
    // 2. Inicializar Frente: Encendemos y cambiamos su dirección a 0x30
    digitalWrite(TOF_FRONT_XSHUT, HIGH); 
    delay(10);
    if(tofFrente.begin(0x30)) { Serial.println("Láser Frente: OK (0x30)"); }
    
    // 3. Inicializar Izquierda: Encendemos y cambiamos a 0x31
    digitalWrite(TOF_LEFT_XSHUT, HIGH); 
    delay(10);
    if(tofIzquierda.begin(0x31)) { Serial.println("Láser Izquierda: OK (0x31)"); }
    
    // 4. Inicializar Derecha: Encendemos y cambiamos a 0x32
    // IMPORTANTE: Cambiamos a 0x32 para que no choque con la brújula en 0x29
    digitalWrite(TOF_RIGHT_XSHUT, HIGH); 
    delay(10);
    if(tofDerecha.begin(0x32)) { Serial.println("Láser Derecha: OK (0x32)"); }
}

void inicializarLinea_QTR8A() {
    qtr.setTypeAnalog();
    qtr.setSensorPins(SensorPin, 6);
}

float leerRumboBrujula() {
    sensors_event_t event; 
    brujula.getEvent(&event); 
    return event.orientation.x; 
}

bool detectarOponenteFrente() {
    VL53L0X_RangingMeasurementData_t medicion; 
    tofFrente.rangingTest(&medicion, false);
    // El láser detecta oponente si hay algo a menos de 20cm (200mm)
    if (medicion.RangeStatus != 4 && medicion.RangeMilliMeter < 200) return true;
    return false;
}

bool detectarLineaBlanca() {
    qtr.read(valoresSensor);
    for (uint8_t i = 0; i < 6; i++) {
        // Ajusta este valor (200) según tu prueba de calibración QTR
        if (valoresSensor[i] < 200) return true; 
    }
    return false;
}