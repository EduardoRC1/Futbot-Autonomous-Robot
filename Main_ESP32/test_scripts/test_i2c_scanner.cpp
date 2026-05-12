// I2C Scanner — usa los pines definidos en Config.h (SDA=16, SCL=17)
#include <Arduino.h>
#include <Wire.h>

// Pines I2C del robot (deben coincidir con Config.h)
static const int PIN_SDA = 16;
static const int PIN_SCL = 17;

void setup() {
    Serial.begin(115200);
    delay(2000);
    Wire.begin(PIN_SDA, PIN_SCL, 100000);
    Wire.setTimeOut(100);
    Serial.printf("I2C Scanner — SDA=%d SCL=%d\n", PIN_SDA, PIN_SCL);
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