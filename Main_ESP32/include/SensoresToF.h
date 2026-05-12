#ifndef SENSORES_TOF_H
#define SENSORES_TOF_H

// ============================================================================
//  SensoresToF.h — Sensores VL53L0X Time-of-Flight
// ============================================================================

#include <Arduino.h>

struct LecturasToF {
    uint16_t frenteMM;
    uint16_t izquierdaMM;
    uint16_t derechaMM;
    bool frenteValida;
    bool izquierdaValida;
    bool derechaValida;
};

// Secuencia XSHUT para asignar direcciones únicas a los tres sensores.
// Devuelve true si al menos un sensor se inicializó correctamente.
bool inicializarSensoresToF();

// Lee los tres sensores y llena la estructura.
LecturasToF leerSensoresToF();

// Imprime las lecturas por Serial.
void imprimirLecturasToF(const LecturasToF& lecturas);

// Devuelve true si hay un objeto a menos de TOF_UMBRAL_OPONENTE_MM al frente.
bool detectarOponenteFrente();

#endif // SENSORES_TOF_H
