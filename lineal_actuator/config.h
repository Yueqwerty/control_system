// Config.h

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// === Definición de Pines ===

// Pines del Motor Paso a Paso con TB6560
const uint8_t MOTOR_PUL_PIN = 3;        // Pin para el pulso
const uint8_t MOTOR_DIR_PIN = 4;        // Pin para la dirección

// Pines del Sensor Ultrasónico HC-SR04
const uint8_t SENSOR_TRIG_PIN = 9;      // Pin de Trigger
const uint8_t SENSOR_ECHO_PIN = 10;     // Pin de Echo

// === Parámetros del Sistema ===

// Distancias objetivo (en centímetros)
const float DISTANCE_LOWER_TARGET = 7.0;     // Distancia para detenerse al bajar
const float DISTANCE_UPPER_TARGET = 35.0;    // Distancia para detenerse al subir
const float DISTANCE_MARGIN = 0.5;           // Margen para evitar oscilaciones

// Velocidad del Motor
const unsigned long PULSE_INTERVAL_DEFAULT_US = 800; // Intervalo entre pulsos en micros (ajustar para la velocidad)
unsigned long pulseInterval = PULSE_INTERVAL_DEFAULT_US;

// Intervalo de lectura del sensor ultrasónico
const unsigned long SENSOR_READ_INTERVAL_MS = 100; // Intervalo entre mediciones en milisegundos

// Timeout para la lectura del sensor ultrasónico
const unsigned long ULTRASONIC_TIMEOUT_US = 30000; // Timeout en micros (30 ms)

// === Definición de Estados ===
enum class MotorState {
  MOVING_DOWN,
  MOVING_UP,
  IDLE
};

// === Configuración de Logging ===
#define LOG_VERBOSITY 2 // 0: Off, 1: Error, 2: Info, 3: Debug

#if LOG_VERBOSITY >= 1
  #define LOG_ERROR(msg) Serial.println(F(msg))
#else
  #define LOG_ERROR(msg)
#endif

#if LOG_VERBOSITY >= 2
  #define LOG_INFO(msg) Serial.println(F(msg))
#else
  #define LOG_INFO(msg)
#endif

#if LOG_VERBOSITY >= 3
  #define LOG_DEBUG(msg) Serial.println(F(msg))
#else
  #define LOG_DEBUG(msg)
#endif

#endif // CONFIG_H
