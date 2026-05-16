#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>

#include "AppConfig.h"

namespace robotarm
{
  class ServoManager
  {
  public:
    void begin();
    void update();

    size_t count() const;
    bool isValidIndex(size_t servoIndex) const;
    const ServoConfig &config(size_t servoIndex) const;
    int angle(size_t servoIndex) const;
    int target(size_t servoIndex) const;
    int direction(size_t servoIndex) const;

    float speed() const;
    void setSpeed(float degreesPerSecond);

    void setTarget(size_t servoIndex, int angle);
    void setDirection(size_t servoIndex, int direction);

  private:
    int clampAngle(size_t servoIndex, int angle) const;

    Servo _servos[kServoCount];
    float _currentAngles[kServoCount] = {};
    int _targetAngles[kServoCount] = {};
    int8_t _directions[kServoCount] = {};
    int _lastWritten[kServoCount] = {};
    uint32_t _lastUpdateMicros = 0;
    float _speed = 60.0f;
  };
}
