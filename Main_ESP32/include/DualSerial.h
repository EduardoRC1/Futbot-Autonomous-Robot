#ifndef DUAL_SERIAL_H
#define DUAL_SERIAL_H

// ============================================================================
//  DualSerial.h — Envía mensajes por USB Serial Y Bluetooth Serial a la vez.
//  Incluir este archivo en cualquier .cpp que necesite imprimir diagnósticos.
// ============================================================================

#include <Arduino.h>
#include "BluetoothSerial.h"

extern BluetoothSerial SerialBT;

// Print + println dual
inline void dualPrint(const char* msg)   { Serial.print(msg);   SerialBT.print(msg); }
inline void dualPrintln(const char* msg) { Serial.println(msg); SerialBT.println(msg); }
inline void dualPrintln()                { Serial.println();    SerialBT.println(); }

// Printf dual (hasta 256 chars)
inline void dualPrintf(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
inline void dualPrintf(const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    Serial.print(buf);
    SerialBT.print(buf);
}

#endif // DUAL_SERIAL_H
