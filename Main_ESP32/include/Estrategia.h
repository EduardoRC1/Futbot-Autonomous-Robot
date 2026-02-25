#ifndef ESTRATEGIA_H
#define ESTRATEGIA_H

#include <Arduino.h>

// nuestro robot jugara como DEFENSA, por lo que su estrategia se basara en defender nuestra porteria y despejar el balon hacia la porteria rival
enum EstadoRobot {
    ESPERANDO_EN_ZONA, // Patrullando la zona
    INTERCEPTANDO, // Entra a nuestra zona para interceptar al rival
    DESPEJANDO, // El balon esta en nuestra zona, lo despejamos hacia la zona del rival
    REGRESANDO_A_BASE, // Volver a la porteria despues de despejar
    EVADIENDO_LINEA, // El robot esta cerca de la linea, se aleja para evitar salirse
    EVADIENDO_RIVAL // El robot esta cerca del rival, se aleja para evitar colisionar
};

void inicilizarEstrtategia();
void evaluarEntorno();
void ejecutarJugadaActual();

#endif // ESTRATEGIA_H

