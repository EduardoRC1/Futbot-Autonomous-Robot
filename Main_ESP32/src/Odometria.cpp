#include "Odometria.h"
#include "SensorIMU.h"
#include "Config.h"
#include <math.h>

static const float DISTANCIA_POR_PULSO =
    (PI * DIAMETRO_LLANTA_CM) / PULSOS_POR_VUELTA;

static volatile long pulsosIzquierdos = 0;
static volatile long pulsosDerechos   = 0;

static long pulsosIzqAnt = 0;
static long pulsosDerAnt = 0;

static float posicionX = POS_INICIAL_X;
static float posicionY = POS_INICIAL_Y;

static void IRAM_ATTR contarPulsoIzq() { pulsosIzquierdos++; }
static void IRAM_ATTR contarPulsoDer() { pulsosDerechos++; }

void inicializarOdometria() {
    pinMode(PIN_ENCODER_IZQ_A, INPUT_PULLUP);
    pinMode(PIN_ENCODER_DER_A, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_IZQ_A), contarPulsoIzq, RISING);
    attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_DER_A), contarPulsoDer, RISING);
    Serial.println("[Odometria] Inicializada");
}

void actualizarPosicion() {
    long deltaIzq = pulsosIzquierdos - pulsosIzqAnt;
    long deltaDer = pulsosDerechos   - pulsosDerAnt;
    pulsosIzqAnt  = pulsosIzquierdos;
    pulsosDerAnt  = pulsosDerechos;

    float distCentro    = ((deltaIzq + deltaDer) / 2.0f) * DISTANCIA_POR_PULSO;
    float anguloRadianes = leerRumboBrujula() * (PI / 180.0f);

    posicionX += distCentro * cosf(anguloRadianes);
    posicionY += distCentro * sinf(anguloRadianes);
}

float obtenerCoordenadaX() { return posicionX; }
float obtenerCoordenadaY() { return posicionY; }
