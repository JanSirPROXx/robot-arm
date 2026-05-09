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

    size_t count() const;
    bool isValidIndex(size_t servoIndex) const;
    const ServoConfig &config(size_t servoIndex) const;
    int angle(size_t servoIndex) const;
    int setAngle(size_t servoIndex, int angle);

  private:
    int clampAngle(size_t servoIndex, int angle) const;

    Servo _servos[kServoCount];
    int _currentAngles[kServoCount] = {};
  };
}
