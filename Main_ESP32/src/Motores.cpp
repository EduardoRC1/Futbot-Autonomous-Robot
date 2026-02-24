#include "Motores.h" // enlaza con el header que define las funciones y pines de los motores

// 1. Asignar los pines para los motores (estos son ejemplos, el programador debe ajustarlos a su hardware)
const int M1_R_EN = 12;
const int M1_L_EN = 13;
const int M1_R_PWM = 14;
const int M1_L_PWM = 27;

const int M2_R_EN = 25;
const int M2_L_EN = 26;
const int M2_R_PWM = 32;
const int M2_L_PWM = 33;

// 2. Escribir la función de inicialización de los motores
void inicializarMotores() {
    // Establecer los pines de control de los motores como salidas
    pinMode(M1_R_EN, OUTPUT);
    pinMode(M1_L_EN, OUTPUT);
    pinMode(M1_R_PWM, OUTPUT);
    pinMode(M1_L_PWM, OUTPUT);
    
    // Activar los motores (esto puede variar según el controlador de motor, el programador debe ajustar esta lógica a su hardware)
    digitalWrite(M1_R_EN, HIGH);
    digitalWrite(M1_L_EN, HIGH);
    
    // El programador puede agregar lógica adicional aquí para configurar los motores (por ejemplo, establecer la dirección inicial o probar los motores)
}

// 3. Escribir las funciones para controlar el movimiento del robot
void moverRobot(int velocidadIzquierda, int velocidadDerecha) {
    // El programador escribira la lógica para convertir las velocidades deseadas de cada motor (que pueden ser valores entre -255 y 255)
    // Senales PWM para controlar la velocidad y dirección de los motores
}

void detenerRobot() {
    // Logica para detener ambos motores (por ejemplo, estableciendo las señales PWM a 0)
}