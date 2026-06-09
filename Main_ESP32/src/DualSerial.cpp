#include "DualSerial.h"
#include "BluetoothSerial.h"
#include <stdarg.h>

static BluetoothSerial SerialBT;

void inicializarDualSerial() {
    SerialBT.begin("Futbot_Monitor");
}

void dualPrint(const char* msg) {
    Serial.print(msg);
    SerialBT.print(msg);
}

void dualPrintln(const char* msg) {
    Serial.println(msg);
    SerialBT.println(msg);
}

void dualPrintln() {
    Serial.println();
    SerialBT.println();
}

void dualPrintf(const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    Serial.print(buf);
    SerialBT.print(buf);
}
