# Futbot-Autonomous-Robot
Base de código C++ principal para el robot de fútbol autónomo con doble ESP32.
# Autonomous Striker - Proyecto FutbotMX 2026

## Descripción del Proyecto
Este repositorio contiene el código fuente oficial de nuestro robot futbolista totalmente autónomo, diseñado y programado desde Matamoros para competir en el evento Futbot en la Ciudad de México en junio de 2026.

El sistema utiliza una arquitectura de **"doble cerebro"** comunicada vía protocolo inalámbrico (ESP-NOW) para separar la carga de procesamiento de la visión artificial del control físico estricto de los motores y la evasión de obstáculos.

## Arquitectura de Hardware
* **Cerebro Central (Navegación y Control):** ESP32 DevKit V1 (30 pines)
* **Sistema de Visión:** ESP32-CAM con módulo de cámara OV2640
* **Tracción y Locomoción:** 2x Motorreductores JGB37-520B (600 RPM con Encoders) controlados por Drivers de alta potencia BTS7960
* **Navegación Inercial:** IMU BNO055 (Giroscopio/Brújula absoluta)
* **Sensores de Entorno:** 3x Láseres VL53L0X (Evasión ToF de oponentes) y 1x Matriz Infrarroja QTR-8A (Detección de límites de cancha)
* **Alimentación:** Batería LiPo 3S 11.1V 2200mAh con Regulador Buck LM2596 (5V para lógica)

##  Estructura del Repositorio
-  `/Main_ESP32` - Bucle central. Control PID de motores, odometría, lectura de sensores I2C y lógica general del partido.
-  `/Vision_ESP32_CAM` - Subsistema de procesamiento de imágenes. Filtra fotogramas en formato RGB565 para rastrear el balón naranja y calcular vectores.
-  `/Docs` - Diagramas de cableado eléctrico, modelos 3D del chasis en PETG y reglas oficiales del torneo.

##  Instrucciones para el Equipo de Programación
1. Clonen este repositorio en su máquina local usando Git.
2. Instalen **VS Code** e instalen la extensión de **PlatformIO**.
3. Abran la carpeta `Main_ESP32` o `Vision_ESP32_CAM` como un proyecto de PlatformIO. El archivo `platformio.ini` descargará automáticamente todas las librerías correctas (Adafruit, QTRSensors, ESP32Encoder) para evitar conflictos de versiones.
4. **Regla de Oro:** NUNCA hagan "commit" directamente a la rama `main`. 
5. Para empezar a programar, creen una nueva rama para su tarea específica (ej. `feature/control-pid-motores` o `feature/filtro-color-camara`) y al terminar, envíen un **Pull Request**.
