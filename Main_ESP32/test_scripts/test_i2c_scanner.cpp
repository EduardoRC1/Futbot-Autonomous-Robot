// I2C Scanner
// Este programa escanea el bus I2C para encontrar dispositivos conectados
#include <Arduino.h>
#include <Wire.h>

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Serial.println("I2C Scanner Iniciando");
}

// Escanea el I2C para encontrar dispositivos conectados y muestra sus direcciones en el monitor serial
// Si no se encuentra, se muestra un mensaje indicando que no se encuentran dispositivos I2C
void loop() {
    byte error, address;
    int nDevices = 0;
    Serial.println("Buscando dispositivos");

    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.println("Dispositivo I2C encontrado en la direccion 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
            Serial.println("!");
            nDevices++;
        }
    }

    if (nDevices == 0) {
        Serial.println("No se encontraron dispositivos I2C");
    }

    Serial.println("---------------");
}