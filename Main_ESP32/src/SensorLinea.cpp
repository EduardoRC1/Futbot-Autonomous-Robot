#include "SensorLinea.h"
#include "Config.h"
#include <QTRSensors.h>

static QTRSensors qtr;
static uint16_t valoresSensor[QTR_NUM_CANALES];
static const uint8_t pinesQTR[QTR_NUM_CANALES] = { PIN_QTR_1, PIN_QTR_2 };
static bool lineaActiva = false;

void inicializarSensorLinea() {
    Serial.println("[Linea] Iniciando QTR-8A...");
    qtr.setTypeAnalog();
    qtr.setSensorPins(pinesQTR, QTR_NUM_CANALES);
    lineaActiva = true;
    Serial.printf("[Linea] QTR OK (%d canales en VP=%d, VN=%d, umbral=%d)\n",
                  QTR_NUM_CANALES, PIN_QTR_1, PIN_QTR_2, QTR_UMBRAL_LINEA);
}

uint16_t obtenerValorQTR(uint8_t canal) {
    if (!lineaActiva || canal >= QTR_NUM_CANALES) return 0;
    return valoresSensor[canal];
}

bool detectarLineaBlanca() {
    if (!lineaActiva) return false;

    qtr.read(valoresSensor);
    for (uint8_t i = 0; i < QTR_NUM_CANALES; i++) {
        if (valoresSensor[i] > QTR_UMBRAL_LINEA) {
            return true;
        }
    }
    return false;
}
