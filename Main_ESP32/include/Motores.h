#ifndef MOTORES_H
#define MOTORES_H

// ============================================================================
//  Motores.h — Control de motores BTS7960
// ============================================================================

void inicializarMotores();
void moverMotores(int velocidadIzquierda, int velocidadDerecha);
void detenerRobot();
void verificarWatchdogMotores();
void pivotearDerecha(int velocidad);
void pivotearIzquierda(int velocidad);

// Giro suave: una llanta se detiene y la otra avanza (giro amplio, no en su lugar)
void girarSuaveDerecha(int velocidad);
void girarSuaveIzquierda(int velocidad);

#endif // MOTORES_H
