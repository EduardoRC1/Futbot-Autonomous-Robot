#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include "Sensores.h"

// 1. DEFINICIÓN DE PINES (Esto conecta con el extern del .h)
const int TOF_FRONT_XSHUT = 5;
const int TOF_LEFT_XSHUT  = 32;
const int TOF_RIGHT_XSHUT = 15;

// Instancias de los sensores
Adafruit_VL53L0X sensorFrente = Adafruit_VL53L0X();
Adafruit_VL53L0X sensorIzq    = Adafruit_VL53L0X();
Adafruit_VL53L0X sensorDer    = Adafruit_VL53L0X();

void inicializarBusI2C() {
    Serial.println("Iniciando BUS I2C en PINES NUEVOS (16 y 17)...");
    // Forzamos el bus a los pines 16 (SDA) y 17 (SCL)
    bool ok = Wire.begin(16, 17, 100000); 
    
    if (!ok) {
        Serial.println("ERROR: No se pudo iniciar el bus en pines 16/17");
    } else {
        Serial.println("Bus I2C iniciado correctamente.");
    }
}

void inicializarToF_VL53L0X() {
    // Usamos las constantes que definimos arriba
    pinMode(TOF_FRONT_XSHUT, OUTPUT); digitalWrite(TOF_FRONT_XSHUT, LOW);
    pinMode(TOF_LEFT_XSHUT, OUTPUT);  digitalWrite(TOF_LEFT_XSHUT, LOW);
    pinMode(TOF_RIGHT_XSHUT, OUTPUT); digitalWrite(TOF_RIGHT_XSHUT, LOW);
    delay(20);

    // Encender y direccionar Frente
    digitalWrite(TOF_FRONT_XSHUT, HIGH);
    delay(20);
    sensorFrente.begin(0x30);

    // Encender y direccionar Izquierda
    digitalWrite(TOF_LEFT_XSHUT, HIGH);
    delay(20);
    sensorIzq.begin(0x31);

    // Encender y direccionar Derecha
    digitalWrite(TOF_RIGHT_XSHUT, HIGH);
    delay(20);
    sensorDer.begin(0x32);
    
    Serial.println("Sensores ToF direccionados.");
}

void obtenerLecturasDetalladas() {
    VL53L0X_RangingMeasurementData_t medida;
    
    Serial.print("Distancias -> ");
    
    // Lectura Frente
    sensorFrente.rangingTest(&medida, false);
    Serial.print("F: "); 
    if(medida.RangeStatus != 4) Serial.print(medida.RangeMilliMeter);
    else Serial.print("Out");
    
    Serial.print("mm | ");

    // Lectura Izquierda
    sensorIzq.rangingTest(&medida, false);
    Serial.print("I: "); 
    if(medida.RangeStatus != 4) Serial.print(medida.RangeMilliMeter);
    else Serial.print("Out");
    
    Serial.print("mm | ");

    // Lectura Derecha (FALTABA ESTA PARTE)
    sensorDer.rangingTest(&medida, false);
    Serial.print("D: "); 
    if(medida.RangeStatus != 4) Serial.print(medida.RangeMilliMeter);
    else Serial.print("Out");

    Serial.println("mm");
}

// 2. FUNCIONES "DUMMY" (Para que el compilador no marque error al buscar lo que declaraste en el .h)
void inicializarIMU_BNO055() { Serial.println("IMU: Pendiente de configurar."); }
void inicializarLinea_QTR8A() { Serial.println("QTR: Pendiente de configurar."); }
float leerRumboBrujula() { return 0.0; }
bool detectarOponenteFrente() { return false; }
bool detectarLineaBlanca() { return false; }