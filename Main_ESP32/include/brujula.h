#ifndef BRUJULA_H
#define BRUJULA_H

#include <Wire.h>
#include <MechaQMC5883.h> // Librería común para el sensor QMC5883L

class Brujula {
  private:
    MechaQMC5883 qmc;
    float declinacion; // Ángulo de ajuste según tu ubicación geográfica

  public:
    /**
     * @brief Constructor de la clase
     * @param declinacionMag Ajuste en grados (ej: -0.21 para CDMX)
     */
    Brujula(float declinacionMag = 0.0) {
      declinacion = declinacionMag;
    }

    /**
     * @brief Inicializa el sensor y la comunicación I2C
     */
    void iniciar() {
      Wire.begin();
      qmc.init();
      // Se puede añadir calibración aquí si es necesario
    }

    /**
     * @brief Calcula el ángulo actual en grados (0-360)
     * @return float Grados respecto al Norte
     */
    float obtenerAzimut() {
      int x, y, z;
      qmc.read(&x, &y, &z);

      // Calculamos el ángulo usando la arcotangente de los ejes X e Y
      float angulo = atan2(y, x);

      // Aplicamos la declinación magnética (convertida a radianes)
      angulo += declinacion;

      // Corregir ángulos menores a 0 o mayores a 2*PI
      if (angulo < 0) angulo += 2 * PI;
      if (angulo > 2 * PI) angulo -= 2 * PI;

      // Convertir de radianes a grados
      return angulo * 180 / PI;
    }
};

#endif