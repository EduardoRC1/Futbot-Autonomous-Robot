// Sensores.cpp
// Universidad de Matamoros — Futbot Autonomous Robot
// Revisado: I2C fix — VL53L0X address sequencing corregido,
//           Wire.begin() centralizado, GPIO15 advertencia documentada.

#include "Sensores.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_VL53L0X.h>
#include <QTRSensors.h>

// ---------------------------------------------------------------------------
// PINES XSHUT — controlan cuál sensor VL53L0X está activo en el bus I2C
// NOTA: GPIO15 es un "strapping pin" en el ESP32.
//       Si tienes problemas para flashear el robot, mueve TOF_RIGHT_XSHUT
//       a un pin libre como D25 o D26 y actualiza el pin aquí y en el esquema.
// ---------------------------------------------------------------------------
const int TOF_FRONT_XSHUT = 5;   // U5 — Láser Frente
const int TOF_LEFT_XSHUT  = 32;  // U7 — Láser Izquierda
const int TOF_RIGHT_XSHUT = 15;  // U6 — Láser Derecha (¡strapping pin! ver nota arriba)

// ---------------------------------------------------------------------------
// DIRECCIONES I2C FINALES (después de la secuencia de asignación):
//   0x28 → BNO055  (dirección de fábrica, no cambia)
//   0x30 → Láser Frente  (U5, reasignado desde 0x29)
//   0x31 → Láser Izquierda (U7, reasignado desde 0x29)
//   0x32 → Láser Derecha  (U6, reasignado desde 0x29)
// ---------------------------------------------------------------------------
Adafruit_BNO055    brujula      = Adafruit_BNO055(55, 0x28, &Wire);
Adafruit_VL53L0X   tofFrente;
Adafruit_VL53L0X   tofIzquierda;
Adafruit_VL53L0X   tofDerecha;
QTRSensors         qtr;

// GPIO34, 35, 36, 39 son input-only en el ESP32 — válidos para lectura analógica QTR.
// No llamar pinMode() con OUTPUT sobre estos pines.
const uint8_t SensorPin[5] = {34, 35, 36, 39, 33};
uint16_t valoresSensor[5];

// ---------------------------------------------------------------------------
// 1. INICIALIZACIÓN DEL BUS I2C
//    Llamar PRIMERO, antes de cualquier sensor.
//    Forzamos SDA=21, SCL=22 (confirmado en esquema EasyEDA).
// ---------------------------------------------------------------------------
void inicializarBusI2C() {
    Wire.begin(21, 22);
    Wire.setClock(400000); // Fast-mode 400kHz — mejora estabilidad con 4 dispositivos
    Serial.println("Bus I2C iniciado: SDA=21, SCL=22 @ 400kHz");
}

// ---------------------------------------------------------------------------
// 2. INICIALIZACIÓN BNO055
//    Llamar DESPUÉS de inicializarToF_VL53L0X() para no reiniciar el bus.
// ---------------------------------------------------------------------------
void inicializarIMU_BNO055() {
    delay(500); // El BNO055 necesita tiempo para arrancar después del power-on
    if (brujula.begin()) {
        brujula.setExtCrystalUse(true);
        Serial.println("BNO055 OK en 0x28");
    } else {
        // No bloqueamos el código — el robot puede operar sin brújula en emergencia
        Serial.println("AVISO: BNO055 no detectado. Revisar conexiones SDA/SCL y dirección 0x28.");
    }
}

// ---------------------------------------------------------------------------
// 3. INICIALIZACIÓN VL53L0X — SECUENCIA CRÍTICA
//
//    Los tres sensores arrancan en la misma dirección I2C por defecto (0x29).
//    Si se despiertan juntos, colisionan en el bus y el ESP32 reporta errores I2C.
//
//    Secuencia correcta:
//      a) Apagar TODOS los sensores vía XSHUT (LOW).
//      b) Despertar uno a la vez, conectar en 0x29, reasignar dirección única.
//      c) Con esa dirección ocupada, el siguiente sensor puede despertar sin colisión.
// ---------------------------------------------------------------------------
void inicializarToF_VL53L0X() {
    // Configurar pines XSHUT como salida
    pinMode(TOF_FRONT_XSHUT, OUTPUT);
    pinMode(TOF_LEFT_XSHUT,  OUTPUT);
    pinMode(TOF_RIGHT_XSHUT, OUTPUT);

    // Paso a) — Apagar todos. Todos quedan en reset, bus limpio.
    digitalWrite(TOF_FRONT_XSHUT, LOW);
    digitalWrite(TOF_LEFT_XSHUT,  LOW);
    digitalWrite(TOF_RIGHT_XSHUT, LOW);
    delay(10);

    // Paso b1) — Despertar Frente (U5), asignar 0x30
    digitalWrite(TOF_FRONT_XSHUT, HIGH);
    delay(10);
    if (!tofFrente.begin()) { // begin() sin argumento = conecta en 0x29 por defecto
        Serial.println("ERROR: Láser Frente no responde en 0x29. Revisar cableado U5.");
    } else {
        tofFrente.setAddress(0x30); // Reasignar a dirección única
        Serial.println("Láser Frente OK → reasignado a 0x30");
    }

    // Paso b2) — Despertar Izquierda (U7), asignar 0x31
    // El Frente ya ocupa 0x30, así que este despierta solo en 0x29 sin conflicto.
    digitalWrite(TOF_LEFT_XSHUT, HIGH);
    delay(10);
    if (!tofIzquierda.begin()) {
        Serial.println("ERROR: Láser Izquierda no responde en 0x29. Revisar cableado U7.");
    } else {
        tofIzquierda.setAddress(0x31);
        Serial.println("Láser Izquierda OK → reasignado a 0x31");
    }

    // Paso b3) — Despertar Derecha (U6), asignar 0x32
    digitalWrite(TOF_RIGHT_XSHUT, HIGH);
    delay(10);
    if (!tofDerecha.begin()) {
        Serial.println("ERROR: Láser Derecha no responde en 0x29. Revisar cableado U6.");
    } else {
        tofDerecha.setAddress(0x32);
        Serial.println("Láser Derecha OK → reasignado a 0x32");
    }
}

// ---------------------------------------------------------------------------
// 4. INICIALIZACIÓN QTR-8A (sensor de línea infrarrojo analógico)
//    5 sensores para evitar conflicto con GPIO32 (usado por TOF_LEFT_XSHUT).
// ---------------------------------------------------------------------------
void inicializarLinea_QTR8A() {
    qtr.setTypeAnalog();
    qtr.setSensorPins(SensorPin, 5);
    Serial.println("QTR-8A configurado con 5 sensores analógicos.");
}

// ---------------------------------------------------------------------------
// FUNCIONES DE LECTURA
// ---------------------------------------------------------------------------

// Devuelve el rumbo del robot en grados (0–360) según la brújula BNO055.
float leerRumboBrujula() {
    sensors_event_t event;
    brujula.getEvent(&event);
    return event.orientation.x;
}

// Devuelve true si hay un objeto a menos de 200 mm frente al robot.
bool detectarOponenteFrente() {
    VL53L0X_RangingMeasurementData_t medicion;
    tofFrente.rangingTest(&medicion, false);
    // RangeStatus == 4 significa "fuera de rango" / sin lectura válida
    if (medicion.RangeStatus != 4 && medicion.RangeMilliMeter < 200) {
        return true;
    }
    return false;
}

// Devuelve true si algún sensor del arreglo detecta la línea blanca del borde.
// Umbral 200: valores bajos = superficie blanca (reflectante).
// Ajustar este valor si la cancha tiene diferente contraste de superficie.
bool detectarLineaBlanca() {
    qtr.read(valoresSensor);
    for (uint8_t i = 0; i < 5; i++) {
        if (valoresSensor[i] < 200) {
            return true;
        }
    }
    return false;
}