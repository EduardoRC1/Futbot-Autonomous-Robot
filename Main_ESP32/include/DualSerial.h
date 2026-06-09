#ifndef DUAL_SERIAL_H
#define DUAL_SERIAL_H

// ============================================================================
//  DualSerial.h — Imprimir por Serial USB y Bluetooth al mismo tiempo
//
//  Todas las funciones envían a Serial y a SerialBT simultáneamente.
//  Incluir este header en cualquier módulo que necesite imprimir diagnóstico.
// ============================================================================

#include <Arduino.h>

// Inicializar el canal Bluetooth (llamar una vez en setup())
void inicializarDualSerial();

// Funciones de impresión dual (Serial + Bluetooth)
void dualPrint(const char* msg);
void dualPrintln(const char* msg);
void dualPrintln();   // solo newline
void dualPrintf(const char* fmt, ...);

#endif // DUAL_SERIAL_H
