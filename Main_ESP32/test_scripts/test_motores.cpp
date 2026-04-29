// Script para hacer test de los motores
#include <Arduino.h>

// Pines del BTS7960
const int M1_R_EN = 12; const int M1_L_EN = 13;
const int M1_R_PWM = 14; const int M1_L_PWM = 27;
const int M2_R_EN = 25; const int M2_L_EN = 26;
const int M2_R_PWM = 15; const int M2_L_PWM = 23;

void setup() {
    Serial.begin(115200);
    pinMode(M1_R_EN, OUTPUT); pinMode(M1_L_EN, OUTPUT);
    pinMode(M1_R_PWM, OUTPUT); pinMode(M1_L_PWM, OUTPUT);
    pinMode(M2_R_EN, OUTPUT); pinMode(M2_L_EN, OUTPUT);
    pinMode(M2_R_PWM, OUTPUT); pinMode(M2_L_PWM, OUTPUT);

// Habilitar drivers
    digitalWrite(M1_R_EN, HIGH); digitalWrite(M1_L_EN, HIGH);
    digitalWrite(M2_R_EN, HIGH); digitalWrite(M2_L_EN, HIGH);
}

void loop() {
    Serial.println("Motores ADELANTE a 40% de potencia");
    analogWrite(M1_R_PWM, 100); analogWrite(M1_L_PWM, 0); // Izquierdo
    analogWrite(M2_R_PWM, 100); analogWrite(M2_L_PWM, 0); // Derecho
    delay(3000);

    Serial.println("Motores DETENIDOS");
    analogWrite(M1_R_PWM, 0); analogWrite(M1_L_PWM, 0);
    analogWrite(M2_R_PWM, 0); analogWrite(M2_L_PWM, 0);
    delay(2000);
}