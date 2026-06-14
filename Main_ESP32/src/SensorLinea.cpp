#include "SensorLinea.h"
#include "Config.h"
#include <QTRSensors.h>

static QTRSensors qtr;
static uint16_t valoresSensor[QTR_NUM_CANALES] = { 0 };
static const uint8_t pinesQTR[QTR_NUM_CANALES] = { PIN_QTR_1, PIN_QTR_2 };
static uint16_t baseCampo[QTR_NUM_CANALES] = { 4095, 4095 };  // valor sobre el verde
static int8_t ultimoLadoLinea = -1;  // 0=Q0, 1=Q1, -1=ninguno/ambos

void inicializarSensorLinea() {
    qtr.setTypeAnalog();
    qtr.setSensorPins(pinesQTR, QTR_NUM_CANALES);

    // Calibración: el robot arranca sobre el campo (verde). Promediar varias
    // lecturas fija la base "sin línea". La línea blanca refleja más IR, así
    // que su lectura cae muy por debajo de esta base.
    uint32_t acum[QTR_NUM_CANALES] = { 0 };
    for (uint8_t k = 0; k < 10; k++) {
        qtr.read(valoresSensor);
        for (uint8_t c = 0; c < QTR_NUM_CANALES; c++) acum[c] += valoresSensor[c];
        delay(5);
    }
    for (uint8_t c = 0; c < QTR_NUM_CANALES; c++) baseCampo[c] = acum[c] / 10;

    Serial.printf("[Linea] QTR OK — base verde Q0=%u Q1=%u (línea si cae %u)\n",
                  baseCampo[0], baseCampo[1], QTR_MARGEN_LINEA);
}

uint16_t obtenerValorQTR(uint8_t canal) {
    if (canal >= QTR_NUM_CANALES) return 0;
    return valoresSensor[canal];
}

bool detectarLineaBlanca() {
    qtr.read(valoresSensor);

    // Línea blanca = lectura muy por debajo de la base del campo.
    bool q0 = (valoresSensor[0] + QTR_MARGEN_LINEA < baseCampo[0]);
    bool q1 = (valoresSensor[1] + QTR_MARGEN_LINEA < baseCampo[1]);

    if (q0 && q1)      ultimoLadoLinea = -1;
    else if (q0)       ultimoLadoLinea =  0;
    else if (q1)       ultimoLadoLinea =  1;
    else               ultimoLadoLinea = -1;

    return q0 || q1;
}

int8_t obtenerLadoLinea() { return ultimoLadoLinea; }
