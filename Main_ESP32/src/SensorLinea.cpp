#include "SensorLinea.h"
#include "Config.h"
#include <QTRSensors.h>

static QTRSensors qtr;
static uint16_t valoresSensor[QTR_NUM_CANALES];
static const uint8_t pinesQTR[QTR_NUM_CANALES] = { PIN_QTR_1, PIN_QTR_2 };
static bool lineaActiva = false;
static uint8_t contadorSaturado = 0;

void inicializarSensorLinea() {
    Serial.println("[Linea] Iniciando QTR-8A...");
    qtr.setTypeAnalog();
    qtr.setSensorPins(pinesQTR, QTR_NUM_CANALES);

    // Leer varias veces para verificar que el sensor responde con valores reales
    uint8_t lecturasSaturadas = 0;
    for (uint8_t prueba = 0; prueba < 5; prueba++) {
        qtr.read(valoresSensor);
        if (valoresSensor[0] >= 4090 && valoresSensor[1] >= 4090) {
            lecturasSaturadas++;
        }
        delay(10);
    }

    if (lecturasSaturadas >= 4) {
        lineaActiva = false;
        Serial.printf("[Linea] QTR DESHABILITADO — lecturas saturadas (Q0=%u Q1=%u)\n",
                      valoresSensor[0], valoresSensor[1]);
        Serial.println("[Linea] Sensor no conectado o superficie muy reflectiva");
    } else {
        lineaActiva = true;
        Serial.printf("[Linea] QTR OK (%d canales en VP=%d, VN=%d, umbral=%d)\n",
                      QTR_NUM_CANALES, PIN_QTR_1, PIN_QTR_2, QTR_UMBRAL_LINEA);
    }
}

uint16_t obtenerValorQTR(uint8_t canal) {
    if (!lineaActiva || canal >= QTR_NUM_CANALES) return 0;
    return valoresSensor[canal];
}

bool detectarLineaBlanca() {
    if (!lineaActiva) return false;

    qtr.read(valoresSensor);

    // Si ambos canales están saturados, ignorar (pin flotante o sin sensor)
    if (valoresSensor[0] >= 4090 && valoresSensor[1] >= 4090) {
        contadorSaturado++;
        if (contadorSaturado >= 20) {
            lineaActiva = false;
            Serial.println("[Linea] QTR auto-deshabilitado — saturado persistente");
        }
        return false;
    }
    contadorSaturado = 0;

    for (uint8_t i = 0; i < QTR_NUM_CANALES; i++) {
        if (valoresSensor[i] > QTR_UMBRAL_LINEA) {
            return true;
        }
    }
    return false;
}
