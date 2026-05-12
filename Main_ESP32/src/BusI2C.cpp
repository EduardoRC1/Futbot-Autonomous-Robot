#include "BusI2C.h"
#include "Config.h"
#include <Wire.h>

bool inicializarBusI2C() {
    Serial.println("[I2C] Iniciando bus...");
    Serial.printf("[I2C] SDA=%d  SCL=%d  Freq=%u Hz\n",
                  PIN_I2C_SDA, PIN_I2C_SCL, I2C_FRECUENCIA);

    // Recuperar bus por si quedó colgado de un reset anterior
    recuperarBusI2C();

    bool ok = Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL, I2C_FRECUENCIA);
    if (!ok) {
        Serial.println("[I2C] ERROR: Wire.begin() falló");
        return false;
    }

    Wire.setTimeOut(I2C_TIMEOUT_MS);
    Serial.printf("[I2C] Bus activo (timeout=%u ms)\n", I2C_TIMEOUT_MS);
    return true;
}

int escanearBusI2C() {
    Serial.println("[I2C] Escaneando dispositivos...");
    int encontrados = 0;

    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        uint8_t error = Wire.endTransmission();
        if (error == 0) {
            Serial.printf("[I2C]   -> 0x%02X encontrado\n", addr);
            encontrados++;
        }
    }

    Serial.printf("[I2C] Total: %d dispositivo(s)\n", encontrados);
    return encontrados;
}

void recuperarBusI2C() {
    // Si SDA está en LOW, el bus está colgado.
    // Generar pulsos de reloj en SCL para que el esclavo suelte SDA.
    pinMode(PIN_I2C_SDA, INPUT);
    pinMode(PIN_I2C_SCL, OUTPUT);

    bool sdaLibre = digitalRead(PIN_I2C_SDA);
    if (sdaLibre) {
        // Bus libre, no hace falta recuperar
        pinMode(PIN_I2C_SCL, INPUT);
        return;
    }

    Serial.println("[I2C] SDA colgada — intentando recuperar bus...");
    for (int i = 0; i < 16; i++) {
        digitalWrite(PIN_I2C_SCL, HIGH);
        delayMicroseconds(5);
        digitalWrite(PIN_I2C_SCL, LOW);
        delayMicroseconds(5);
        if (digitalRead(PIN_I2C_SDA)) break;
    }

    // Generar condición STOP (SDA LOW->HIGH mientras SCL está HIGH)
    pinMode(PIN_I2C_SDA, OUTPUT);
    digitalWrite(PIN_I2C_SDA, LOW);
    delayMicroseconds(5);
    digitalWrite(PIN_I2C_SCL, HIGH);
    delayMicroseconds(5);
    digitalWrite(PIN_I2C_SDA, HIGH);
    delayMicroseconds(5);

    // Liberar pines para que Wire los tome
    pinMode(PIN_I2C_SDA, INPUT);
    pinMode(PIN_I2C_SCL, INPUT);

    Serial.println("[I2C] Recuperación completada");
}
