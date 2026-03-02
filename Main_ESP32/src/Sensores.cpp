#include "Sensores.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_VL53L0X.h>
#include <QTRSensors.h>

const int TOF_FRONT_XSHUT = 5; // Este pin se puede cambiar según la conexion final
const int TOF_LEFT_XSHUT = 18; // Este pin se puede cambiar según la conexion final
const int TOF_RIGHT_XSHUT = 19; // Este pin se puede cambiar según la conexion final

Adafruit_BNO055 brujula = Adafruit_BNO055(55, 0x28, &Wire); // El segundo parámetro es la dirección I2C, que puede variar según la configuración del hardware. Verificar con el datasheet o el código de ejemplo de la librería.
Adafruit_VL53L0X tofFrente = Adafruit_VL53L0X(); 
Adafruit_VL53L0X tofIzquierda = Adafruit_VL53L0X();
Adafruit_VL53L0X tofDerecha = Adafruit_VL53L0X();
QTRSensors qtr; // El número de sensores se define al configurar los pines, en este caso 8 sensores analógicos.

// Pines para los sensores de línea (QTR-8A), conectados a los pines analógicos del ESP32. Se puede cambiar segun la conexion final mientras sean pines analogicos
const uint8_t SensorPin[8] = {34, 35, 36, 39, 32, 33, 25, 26}; 
uint16_t valoresSensor[8];

// Esta funcion inicializa el bus I2C, que es necesario para comunicarse con los sensores BNO055 y VL53L0X. Se llama al inicio del programa para configurar la comunicación.
void inicializarBusI2C() { Wire.begin(); }

// Esta función configura el sensor de orientación BNO055. Se llama al inicio del programa para asegurarse de que el sensor esté listo para su uso. Si el sensor se inicializa correctamente, se activa el uso del cristal externo para mejorar la precisión.
void inicializarIMU_BNO055() {
    if (brujula.begin()) {
        brujula.setExtCrystalUse(true);
    }
}

// Esta función inicializa los sensores de distancia VL53L0X. Se configura cada sensor para que tenga una dirección I2C única, lo que permite que el microcontrolador se comunique con cada uno de ellos sin conflictos. Además, se establece la configuración de medición para cada sensor.
void inicializarToF_VL53L0X() {
    pinMode(TOF_FRONT_XSHUT, OUTPUT);
    pinMode(TOF_LEFT_XSHUT, OUTPUT);
    pinMode(TOF_RIGHT_XSHUT, OUTPUT);
    digitalWrite(TOF_FRONT_XSHUT, LOW);
    digitalWrite(TOF_LEFT_XSHUT, LOW);
    digitalWrite(TOF_RIGHT_XSHUT, LOW);
    delay(10);
    digitalWrite(TOF_FRONT_XSHUT, HIGH);
    delay(10);
    tofFrente.begin(0x30); 
    // Repetir para los otros 2 (Código simplificado)
}

// Esta función configura los sensores de línea QTR-8A. Se establece el tipo de sensor como analógico y se asignan los pines correspondientes a cada sensor. Esto permite que el microcontrolador lea los valores de reflectancia de cada sensor para detectar la línea blanca en el suelo.
void inicializarLinea_QTR8A() {
    qtr.setTypeAnalog();
    qtr.setSensorPins(SensorPin, 8);
}

//  Esta función lee el valor de orientación del sensor BNO055. Devuelve el ángulo de rumbo (heading) en grados, que indica la dirección en la que está orientado el robot. Este valor se puede usar para controlar la dirección del robot o para realizar correcciones de trayectoria.
float leerRumboBrujula() {
    sensors_event_t event;
    brujula.getEvent(&event);
    return event.orientation.x; 
}

// Esta función utiliza el sensor de distancia frontal para detectar si hay un oponente frente al robot. Si el sensor detecta un objeto a menos de 200 mm de distancia, se considera que hay un oponente presente. La función devuelve true si se detecta un oponente y false en caso contrario.
bool detectarOponenteFrente() {
    VL53L0X_RangingMeasurementData_t medicion;
    tofFrente.rangingTest(&medicion, false);
    if (medicion.RangeStatus != 4 && medicion.RangeMilliMeter < 200) return true;
    return false;
}

// Esta función utiliza los sensores de distancia laterales para detectar si hay oponentes a los lados del robot. Si alguno de los sensores detecta un objeto a menos de 200 mm de distancia, se considera que hay un oponente presente. La función devuelve true si se detecta un oponente en cualquiera de los lados y false si no se detecta ningún oponente.
bool detectarLineaBlanca() {
    qtr.read(valoresSensor);
    for (uint8_t i = 0; i < 8; i++) {
        if (valoresSensor[i] < 200) return true; 
    }
    return false;
}
