# Cerebro Central: Código Fuente (.cpp)

Esta carpeta contiene la lógica real del robot. Mientras que los archivos `.h` solo declardan *qué* existe, estos archivos `.cpp` definen *cómo* funciona todo. Aquí es donde programamos los motores, leemos los sensores y ejecutamos la estrategia de RoboCup.

### Guía de Archivos:

* **`main.cpp`**: El corazón del programa. Contiene el `setup()` (inicia todo) y el `loop()` (el ciclo infinito que mantiene al robot vivo).
* **`Comunicacion.cpp`**: Contiene la función `alRecibirDatos()`. Aquí se procesa el mensaje inalámbrico que llega de la cámara (balón, distancia, portería).
* **`ControlPID.cpp`**: Las matemáticas de control suave. Aquí ajustamos las variables `Kp`, `Ki`, y `Kd` para que el robot no patine al acelerar.
* **`Estrategia.cpp`**: La Máquina de Estados. Aquí programamos la lógica de "Sweeper" (Defensa), la evasión de líneas blancas y el sistema de apuntado IMU.
* **`Motores.cpp`**: Aquí enviamos las señales PWM (Pulsos) a los drivers BTS7960 para mover las llantas a diferentes velocidades.
* **`Odometria.cpp`**: La física del robot. Cuenta los "ticks" de los encoders JGB37 y usa trigonometría para actualizar las coordenadas `X` y `Y` en la cancha.
* **`Sensores.cpp`**: Aquí leemos directamente el hardware: la brújula BNO055 (I2C), los láseres VL53L0X (I2C) y el arreglo infrarrojo QTR-8A (Pines Analógicos).

---
**Regla de Oro del Equipo:** Si necesitan crear una nueva función aquí, ¡no olviden agregar su nombre en el archivo `.h` correspondiente dentro de la carpeta `include`!

