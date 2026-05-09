#include "ServoManager.h"

namespace robotarm
{
  void ServoManager::begin()
  {
    for (size_t i = 0; i < kServoCount; ++i)
    {
      _servos[i].setPeriodHertz(50);
      _servos[i].attach(kServoConfigs[i].pin);
      setAngle(i, kServoConfigs[i].initialAngle);
    }
  }

  size_t ServoManager::count() const
  {
    return kServoCount;
  }

  bool ServoManager::isValidIndex(size_t servoIndex) const
  {
    return servoIndex < kServoCount;
  }

  const ServoConfig &ServoManager::config(size_t servoIndex) const
  {
    return kServoConfigs[servoIndex];
  }

  int ServoManager::angle(size_t servoIndex) const
  {
    return _currentAngles[servoIndex];
  }

  int ServoManager::setAngle(size_t servoIndex, int angle)
  {
    const int safeAngle = clampAngle(servoIndex, angle);
    _servos[servoIndex].write(safeAngle);
    _currentAngles[servoIndex] = safeAngle;
    return safeAngle;
  }

  int ServoManager::clampAngle(size_t servoIndex, int angle) const
  {
    const ServoConfig &servo = kServoConfigs[servoIndex];
    if (angle < servo.minAngle)
    {
      return servo.minAngle;
    }
    if (angle > servo.maxAngle)
    {
      return servo.maxAngle;
    }
    return angle;
  }
}
