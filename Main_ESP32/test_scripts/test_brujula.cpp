#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// Crear el objeto del sensor
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28); // 0x28 o 0x29 dependiendo de tu módulo

void setup() {
  Serial.begin(115200);
  Serial.println("Prueba de Brújula BNO055...");
  
  if(!bno.begin()) {
    Serial.println("No se detectó el BNO055. Revisa el I2C Scanner.");
    while(1);
  }
  delay(1000);
  bno.setExtCrystalUse(true);
}

void loop() {
  // Obtener los datos del sensor
  sensors_event_t event;
  bno.getEvent(&event);

  // Obtener el estado de calibración (0 = No calibrado, 3 = Perfectamente calibrado)
  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  Serial.print("Ángulo Z (Heading): ");
  Serial.print(event.orientation.x, 2);
  
  Serial.print("  |  Calibración [Sis:");
  Serial.print(system, DEC);
  Serial.print(" Giro:");
  Serial.print(gyro, DEC);
  Serial.print(" Mag:");
  Serial.print(mag, DEC);
  Serial.println("] (Mueve el robot en 8 si Mag es 0)");
  
  delay(200);
}