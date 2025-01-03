// Logic.cpp

#include "Logic.h"
#include "Config.h"

// Comandos Seriales
const String CMD_AUTO = "AUTO";
const String CMD_UP = "UP";
const String CMD_DOWN = "DOWN";
const String CMD_STOP = "STOP";
const String CMD_SET_SPEED = "SET_SPEED";

Logic::Logic(Motor& motor, Sensor& sensor)
  : motor_(motor), sensor_(sensor),
    currentState_(MotorState::IDLE), previousState_(MotorState::IDLE),
    autoMode_(false), previousDistanceMillis_(0),
    currentDistance_(0.0) {}

void Logic::initialize() {
  currentState_ = MotorState::IDLE;
  previousState_ = MotorState::IDLE;
  LOG_INFO("Lógica del sistema inicializada.");
}

void Logic::update() {
  unsigned long currentMillis = millis();

  // Lectura del Sensor Ultrasónico a Intervalos Regulares
  if (currentMillis - previousDistanceMillis_ >= SENSOR_READ_INTERVAL_MS) {
    previousDistanceMillis_ = currentMillis;
    currentDistance_ = sensor_.readDistance();

    // Mostrar la distancia en el monitor serial
    if (currentDistance_ >= 0.0) {
      Serial.print("Distancia actual: ");
      Serial.print(currentDistance_);
      Serial.println(" cm");
    } else {
      Serial.println("Error en la lectura del sensor ultrasónico.");
      // Implementar lógica de manejo de errores si es necesario
    }

    // Controlar el movimiento del motor basado en la distancia y el estado actual
    processState();
  }

  // Actualizar el motor de manera no bloqueante
  motor_.update();
}

void Logic::handleSerialCommands() {
  while (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Eliminar espacios en blanco y saltos de línea

    LOG_INFO("Comando recibido: " + command);

    if (command.equalsIgnoreCase(CMD_AUTO)) {
      // Activar modo automático
      LOG_INFO("Activando modo automático.");
      setAutoMode(true);
      motor_.moveDown();
      currentState_ = MotorState::MOVING_DOWN;
      previousState_ = MotorState::MOVING_DOWN;
    }
    else if (command.equalsIgnoreCase(CMD_UP)) {
      // Mover manualmente hacia arriba
      LOG_INFO("Modo manual: Moviendo hacia arriba.");
      setAutoMode(false);
      motor_.moveUp();
      currentState_ = MotorState::MOVING_UP;
      previousState_ = MotorState::MOVING_UP;
    }
    else if (command.equalsIgnoreCase(CMD_DOWN)) {
      // Mover manualmente hacia abajo
      LOG_INFO("Modo manual: Moviendo hacia abajo.");
      setAutoMode(false);
      motor_.moveDown();
      currentState_ = MotorState::MOVING_DOWN;
      previousState_ = MotorState::MOVING_DOWN;
    }
    else if (command.equalsIgnoreCase(CMD_STOP)) {
      // Detener el motor y desactivar modo automático
      LOG_INFO("Deteniendo motor y desactivando modo automático.");
      setAutoMode(false);
      motor_.stop();
      currentState_ = MotorState::IDLE;
    }
    else if (command.startsWith(CMD_SET_SPEED)) {
      // Comando para ajustar la velocidad del motor
      // Formato esperado: SET_SPEED <valor_en_micros>
      int spaceIndex = command.indexOf(' ');
      if (spaceIndex != -1) {
        String valueStr = command.substring(spaceIndex + 1);
        unsigned long newInterval = valueStr.toInt();
        if (newInterval > 0 && newInterval < 1000000) { // Validar rango
          adjustSpeed(newInterval);
          Serial.print("Intervalo de pulsos ajustado a: ");
          Serial.print(pulseInterval);
          Serial.println(" micros.");
        }
        else {
          LOG_ERROR("Valor de velocidad inválido.");
        }
      }
      else {
        LOG_ERROR("Formato de comando incorrecto para SET_SPEED.");
      }
    }
    else {
      LOG_ERROR("Comando no reconocido.");
    }
  }
}

void Logic::transitionState() {
  switch (currentState_) {
    case MotorState::MOVING_DOWN:
      if (currentDistance_ <= DISTANCE_LOWER_TARGET + DISTANCE_MARGIN) {
        motor_.stop();
        LOG_INFO("¡Distancia de 7 cm alcanzada! Deteniendo motor.");
        currentState_ = MotorState::IDLE;
      }
      break;

    case MotorState::MOVING_UP:
      if (currentDistance_ >= DISTANCE_UPPER_TARGET - DISTANCE_MARGIN) {
        motor_.stop();
        LOG_INFO("¡Distancia de 35 cm alcanzada! Deteniendo motor.");
        currentState_ = MotorState::IDLE;
      }
      break;

    case MotorState::IDLE:
      if (autoMode_) {
        if (previousState_ == MotorState::MOVING_DOWN) {
          // Cambiar a subir después de detenerse
          motor_.moveUp();
          currentState_ = MotorState::MOVING_UP;
          previousState_ = MotorState::MOVING_UP;
        }
        else if (previousState_ == MotorState::MOVING_UP) {
          // Cambiar a bajar después de detenerse
          motor_.moveDown();
          currentState_ = MotorState::MOVING_DOWN;
          previousState_ = MotorState::MOVING_DOWN;
        }
      }
      break;
  }
}

void Logic::processState() {
  transitionState();
}

void Logic::setAutoMode(bool mode) {
  autoMode_ = mode;
}

void Logic::adjustSpeed(unsigned long newInterval) {
  pulseInterval = newInterval;
  motor_.setPulseInterval(newInterval);
}


{}