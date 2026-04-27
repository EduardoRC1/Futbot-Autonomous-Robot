// Test de calibracion del sensor de linea
// Si no se detecta la linea, el valor crudo debe ser alto (cerca de 4095 para un ADC de 12 bits)
#include <Arduino.h>

const int PIN_SENSOR_LINEA = 34;

void setup() {
    Serial.begin(115200);
    pinMode(PIN_SENSOR_LINEA, INPUT);
}

void loop() {
    int valorCrudo = analogRead(PIN_SENSOR_LINEA);
    
    Serial.print("Lectura de Suelo QTR-8A: ");
    Serial.println(valorCrudo);
    
    delay(200); 

    // Para saber la calibracion, coloca el sensor sobre la linea negra y luego sobre el suelo blanco, anotando los valores crudos
