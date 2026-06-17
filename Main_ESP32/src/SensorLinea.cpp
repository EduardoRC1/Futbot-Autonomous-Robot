#include "SensorLinea.h"
#include "Config.h"
#include <Arduino.h>

static uint16_t valoresSensor[QTR_NUM_CANALES] = { 0 };
static const uint8_t pinesQTR[QTR_NUM_CANALES] = { PIN_QTR_1, PIN_QTR_2 };
static uint16_t baseCampo[QTR_NUM_CANALES] = { 0, 0 };
static int8_t ultimoLadoLinea = -1;
static bool qtrFuncional = false;

static void leerCrudo() {
    for (uint8_t c = 0; c < QTR_NUM_CANALES; c++)
        valoresSensor[c] = analogRead(pinesQTR[c]);
}

void inicializarSensorLinea() {
    analogReadResolution(12);
    pinMode(PIN_QTR_1, INPUT);
    pinMode(PIN_QTR_2, INPUT);

    uint32_t acum[QTR_NUM_CANALES] = { 0 };
    for (uint8_t k = 0; k < 10; k++) {
        leerCrudo();
        for (uint8_t c = 0; c < QTR_NUM_CANALES; c++) acum[c] += valoresSensor[c];
        delay(5);
    }
    for (uint8_t c = 0; c < QTR_NUM_CANALES; c++) baseCampo[c] = acum[c] / 10;

    qtrFuncional = (baseCampo[0] > 100 || baseCampo[1] > 100);

    Serial.printf("[Linea] QTR base verde Q0=%u Q1=%u (margen=%u)\n",
                  baseCampo[0], baseCampo[1], QTR_MARGEN_LINEA);
    if (!qtrFuncional)
        Serial.println("[Linea] *** AVISO: lectura ~0 — QTR no funcional. "
                       "Revisar: VCC a 3.3V, señal a GPIO36/39, sensor >3mm del piso ***");
}

uint16_t obtenerValorQTR(uint8_t canal) {
    if (canal >= QTR_NUM_CANALES) return 0;
    return valoresSensor[canal];
}

bool detectarLineaBlanca() {
    leerCrudo();

    if (!qtrFuncional) return false;

    bool q0 = (valoresSensor[0] + QTR_MARGEN_LINEA < baseCampo[0]);
    bool q1 = (valoresSensor[1] + QTR_MARGEN_LINEA < baseCampo[1]);

    if (q0 && q1)      ultimoLadoLinea = -1;
    else if (q0)       ultimoLadoLinea =  0;
    else if (q1)       ultimoLadoLinea =  1;
    else               ultimoLadoLinea = -1;

    return q0 || q1;
}

int8_t obtenerLadoLinea() { return ultimoLadoLinea; }
