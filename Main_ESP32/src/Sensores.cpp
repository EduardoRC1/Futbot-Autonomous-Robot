#include "Sensores.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_VL53L0X.h>
#include <QTRSensors.h>

// Pines de control XSHUT (Confirmados con tu diagrama de EasyEDA)
const int TOF_FRONT_XSHUT = 5;   // U5
const int TOF_LEFT_XSHUT = 32;   // U7
const int TOF_RIGHT_XSHUT = 15;  // U6

// CONFIGURACIÓN DE DIRECCIONES I2C:
// 0x28 -> Brújula (Dirección por defecto BNO055)
// 0x30 -> Láser Frente
// 0x31 -> Láser Izquierda
// 0x32 -> Láser Derecha

Adafruit_BNO055 brujula = Adafruit_BNO055(55, 0x28, &Wire);
Adafruit_VL53L0X tofFrente = Adafruit_VL53L0X(); 
Adafruit_VL53L0X tofIzquierda = Adafruit_VL53L0X();
Adafruit_VL53L0X tofDerecha = Adafruit_VL53L0X();
QTRSensors qtr; 

// Ajustado a 5 sensores para evitar conflicto con pin 32 (XSHUT Izquierdo)
const uint8_t SensorPin[5] = {34, 35, 36, 39, 33}; 
uint16_t valoresSensor[5];

void inicializarBusI2C() { 
    // Forzamos pines 21 y 22 para evitar errores de comunicación
    Wire.begin(21, 22); 
}

void inicializarIMU_BNO055() {
    delay(500); 
    if (brujula.begin()) { 
        brujula.setExtCrystalUse(true); 
        Serial.println("BNO055 Iniciado OK en 0x28");
    } else {
        // No bloqueamos el código si no está conectada
        Serial.println("Aviso: BNO055 no detectado (Omitiendo...)");
    }
}

void inicializarToF_VL53L0X() {
    pinMode(TOF_FRONT_XSHUT, OUTPUT); 
    pinMode(TOF_LEFT_XSHUT, OUTPUT); 
    pinMode(TOF_RIGHT_XSHUT, OUTPUT);

    // 1. Reset físico: Apagamos todos
    digitalWrite(TOF_FRONT_XSHUT, LOW); 
    digitalWrite(TOF_LEFT_XSHUT, LOW); 
    digitalWrite(TOF_RIGHT_XSHUT, LOW);
    delay(10);
    
    // 2. Inicializar Frente (U5)
    digitalWrite(TOF_FRONT_XSHUT, HIGH); 
    delay(10);
    if(tofFrente.begin(0x30)) { Serial.println("Láser Frente: OK (0x30)"); }
    
    // 3. Inicializar Izquierda (U7)
    digitalWrite(TOF_LEFT_XSHUT, HIGH); 
    delay(10);
    if(tofIzquierda.begin(0x31)) { Serial.println("Láser Izquierda: OK (0x31)"); }
    
    // 4. Inicializar Derecha (U6)
    digitalWrite(TOF_RIGHT_XSHUT, HIGH); 
    delay(10);
    if(tofDerecha.begin(0x32)) { Serial.println("Láser Derecha: OK (0x32)"); }
}

void inicializarLinea_QTR8A() {
    qtr.setTypeAnalog();
    // Cambiado a 5 sensores para coincidir con el arreglo SensorPin
    qtr.setSensorPins(SensorPin, 5); 
}

float leerRumboBrujula() {
    sensors_event_t event; 
    brujula.getEvent(&event); 
    return event.orientation.x; 
}

bool detectarOponenteFrente() {
    VL53L0X_RangingMeasurementData_t medicion; 
    tofFrente.rangingTest(&medicion, false);
    // Si hay algo a menos de 200mm, hay oponente
    if (medicion.RangeStatus != 4 && medicion.RangeMilliMeter < 200) return true;
    return false;
}

bool detectarLineaBlanca() {
    qtr.read(valoresSensor);
    // Cambiado el límite a 5 para no leer basura en memoria
    for (uint8_t i = 0; i < 5; i++) {
        // Si el valor es bajo, detectó línea blanca (ajustar según superficie)
        if (valoresSensor[i] < 200) return true; 
    }
    return false;
}