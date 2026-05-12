#ifndef MOTORES_H
#define MOTORES_H

// ============================================================================
//  Motores.h — Control de motores BTS7960
// ============================================================================

void inicializarMotores();
void moverMotores(int velocidadIzquierda, int velocidadDerecha);
void detenerRobot();
void pivotearDerecha(int velocidad);
void pivotearIzquierda(int velocidad);

#endif // MOTORES_H
