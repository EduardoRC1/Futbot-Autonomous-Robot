#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando prueba de VL53L0X...");
  
  if (!lox.begin()) {
    Serial.println("Fallo al iniciar VL53L0X. Revisa los cables SDA/SCL.");
    while(1); // Se detiene aquí si hay error
  }
  Serial.println("VL53L0X Listo. Mueve un objeto frente al sensor.");
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false); 

  if (measure.RangeStatus != 4) {  // 4 significa "fuera de rango"
    Serial.print("Distancia al obstáculo: ");
    Serial.print(measure.RangeMilliMeter);
    Serial.println(" mm");
  } else {
    Serial.println("Fuera de rango (Demasiado lejos)");
  }
  delay(100);
}