#include "SensoresToF.h"
#include "Config.h"
#include "DualSerial.h"
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

// Instancias de los sensores
static Adafruit_VL53L0X sensorFrente;
static Adafruit_VL53L0X sensorIzq;
static Adafruit_VL53L0X sensorDer;

// Banderas de inicialización exitosa
static bool frenteOK  = false;
static bool izqOK     = false;
static bool derOK     = false;

// Últimas lecturas válidas (para detección de oponente)
static uint16_t ultimaLecturaFrente = 9999;
static uint16_t ultimaLecturaIzq    = 9999;
static uint16_t ultimaLecturaDer    = 9999;

// ---------------------------------------------------------------------------
// Inicializa un solo sensor VL53L0X con reintentos
// ---------------------------------------------------------------------------
static bool iniciarUnSensor(Adafruit_VL53L0X& sensor, int pinXshut,
                            uint8_t nuevaDir, const char* nombre) {
    // Levantar XSHUT — el sensor arranca en dirección 0x29
    digitalWrite(pinXshut, HIGH);

    // Esperar a que el sensor arranque su firmware interno.
    // El datasheet dice 1.2 ms mínimo, pero en la práctica el bus necesita
    // más tiempo de estabilización — 150 ms es seguro.
    delay(150);

    // Verificar que el sensor aparece en el bus antes de configurarlo
    Wire.beginTransmission(0x29);
    uint8_t ack = Wire.endTransmission();
    if (ack != 0) {
        dualPrintf("[ToF] %s: no responde en 0x29 (error=%d)\n", nombre, ack);
        return false;
    }

    // Inicializar con reintentos — la librería Adafruit internamente llama
    // Wire.begin() sin argumentos, lo cual en ESP32 reutiliza los pines ya
    // configurados, pero puede fallar si el bus está ocupado.
    for (int intento = 1; intento <= 3; intento++) {
        if (sensor.begin(nuevaDir, false, &Wire)) {
            dualPrintf("[ToF] %s: OK en 0x%02X (intento %d)\n",
                          nombre, nuevaDir, intento);
            return true;
        }
        dualPrintf("[ToF] %s: begin() fallo (intento %d/3)\n", nombre, intento);
        delay(100);
    }

    dualPrintf("[ToF] %s: ERROR — no se pudo inicializar\n", nombre);
    return false;
}

bool inicializarSensoresToF() {
    dualPrintln("[ToF] Inicializando sensores VL53L0X...");

    // 1. Apagar todos los sensores (XSHUT LOW)
    pinMode(PIN_TOF_FRONT_XSHUT, OUTPUT);
    pinMode(PIN_TOF_LEFT_XSHUT, OUTPUT);
    pinMode(PIN_TOF_RIGHT_XSHUT, OUTPUT);

    digitalWrite(PIN_TOF_FRONT_XSHUT, LOW);
    digitalWrite(PIN_TOF_LEFT_XSHUT, LOW);
    digitalWrite(PIN_TOF_RIGHT_XSHUT, LOW);

    // Esperar a que todos se apaguen completamente
    delay(50);

    // 2. Encender uno por uno y asignar dirección única.
    //    Cada sensor arranca en 0x29. Le cambiamos la dirección antes de
    //    encender el siguiente para evitar colisiones.
    frenteOK = iniciarUnSensor(sensorFrente, PIN_TOF_FRONT_XSHUT,
                               TOF_ADDR_FRONT, "Frente");

    izqOK = iniciarUnSensor(sensorIzq, PIN_TOF_LEFT_XSHUT,
                            TOF_ADDR_LEFT, "Izquierda");

    derOK = iniciarUnSensor(sensorDer, PIN_TOF_RIGHT_XSHUT,
                            TOF_ADDR_RIGHT, "Derecha");

    int total = (int)frenteOK + (int)izqOK + (int)derOK;
    dualPrintf("[ToF] %d de 3 sensores activos\n", total);
    return (total > 0);
}

LecturasToF leerSensoresToF() {
    LecturasToF lecturas = {0, 0, 0, false, false, false};
    VL53L0X_RangingMeasurementData_t medida;

    if (frenteOK) {
        sensorFrente.rangingTest(&medida, false);
        if (medida.RangeStatus != 4) {
            lecturas.frenteMM    = medida.RangeMilliMeter;
            lecturas.frenteValida = true;
            ultimaLecturaFrente  = medida.RangeMilliMeter;
        } else {
            ultimaLecturaFrente = 9999;  // fuera de rango = sin rival (no latch)
        }
    }

    if (izqOK) {
        sensorIzq.rangingTest(&medida, false);
        if (medida.RangeStatus != 4) {
            lecturas.izquierdaMM    = medida.RangeMilliMeter;
            lecturas.izquierdaValida = true;
            ultimaLecturaIzq        = medida.RangeMilliMeter;
        } else {
            ultimaLecturaIzq = 9999;  // fuera de rango = sin rival (no latch)
        }
    }

    if (derOK) {
        sensorDer.rangingTest(&medida, false);
        if (medida.RangeStatus != 4) {
            lecturas.derechaMM    = medida.RangeMilliMeter;
            lecturas.derechaValida = true;
            ultimaLecturaDer      = medida.RangeMilliMeter;
        } else {
            ultimaLecturaDer = 9999;  // fuera de rango = sin rival (no latch)
        }
    }

    return lecturas;
}

void imprimirLecturasToF(const LecturasToF& l) {
    char fBuf[16], iBuf[16], dBuf[16];
    if (l.frenteValida)    snprintf(fBuf, sizeof(fBuf), "%umm", l.frenteMM);
    else                   strcpy(fBuf, "---");
    if (l.izquierdaValida) snprintf(iBuf, sizeof(iBuf), "%umm", l.izquierdaMM);
    else                   strcpy(iBuf, "---");
    if (l.derechaValida)   snprintf(dBuf, sizeof(dBuf), "%umm", l.derechaMM);
    else                   strcpy(dBuf, "---");
    dualPrintf("Distancias -> F:%s | I:%s | D:%s\n", fBuf, iBuf, dBuf);
}

bool detectarOponenteFrente() {
    return (frenteOK && ultimaLecturaFrente < TOF_UMBRAL_OPONENTE_FRENTE_MM);
}

bool detectarOponenteIzquierda() {
    return (izqOK && ultimaLecturaIzq < TOF_UMBRAL_OPONENTE_LATERAL_MM);
}

bool detectarOponenteDerecha() {
    return (derOK && ultimaLecturaDer < TOF_UMBRAL_OPONENTE_LATERAL_MM);
}

uint16_t obtenerDistanciaFrente()     { return ultimaLecturaFrente; }
uint16_t obtenerDistanciaIzquierda()  { return ultimaLecturaIzq; }
uint16_t obtenerDistanciaDerecha()    { return ultimaLecturaDer; }

bool tofFrenteActivo()    { return frenteOK; }
bool tofIzquierdaActivo() { return izqOK; }
bool tofDerechaActivo()   { return derOK; }
