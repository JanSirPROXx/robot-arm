#include "ServoManager.h"

#include <math.h>

namespace robotarm
{
  namespace
  {
    constexpr float kMinSpeed = 5.0f;
    constexpr float kMaxSpeed = 300.0f;
  }

  void ServoManager::begin()
  {
    for (size_t i = 0; i < kServoCount; ++i)
    {
      _servos[i].setPeriodHertz(50);
      _servos[i].attach(kServoConfigs[i].pin);
      const int init = clampAngle(i, kServoConfigs[i].initialAngle);
      _currentAngles[i] = static_cast<float>(init);
      _targetAngles[i] = init;
      _directions[i] = 0;
      _lastWritten[i] = init;
      _servos[i].write(init);
    }
    _lastUpdateMicros = micros();
  }

  void ServoManager::update()
  {
    const uint32_t now = micros();
    const uint32_t elapsed = now - _lastUpdateMicros;
    _lastUpdateMicros = now;
    const float dt = elapsed / 1000000.0f;
    const float maxStep = _speed * dt;

    for (size_t i = 0; i < kServoCount; ++i)
    {
      if (_directions[i] != 0)
      {
        const ServoConfig &cfg = kServoConfigs[i];
        _targetAngles[i] = _directions[i] > 0 ? cfg.maxAngle : cfg.minAngle;
      }

      const float target = static_cast<float>(_targetAngles[i]);
      const float current = _currentAngles[i];
      const float diff = target - current;

      if (fabsf(diff) <= maxStep)
      {
        _currentAngles[i] = target;
      }
      else
      {
        _currentAngles[i] = current + (diff > 0 ? maxStep : -maxStep);
      }

      const int writeVal = static_cast<int>(lroundf(_currentAngles[i]));
      if (writeVal != _lastWritten[i])
      {
        _servos[i].write(writeVal);
        _lastWritten[i] = writeVal;
      }
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
    return static_cast<int>(lroundf(_currentAngles[servoIndex]));
  }

  int ServoManager::target(size_t servoIndex) const
  {
    return _targetAngles[servoIndex];
  }

  int ServoManager::direction(size_t servoIndex) const
  {
    return _directions[servoIndex];
  }

  float ServoManager::speed() const
  {
    return _speed;
  }

  void ServoManager::setSpeed(float degreesPerSecond)
  {
    if (degreesPerSecond < kMinSpeed)
    {
      _speed = kMinSpeed;
    }
    else if (degreesPerSecond > kMaxSpeed)
    {
      _speed = kMaxSpeed;
    }
    else
    {
      _speed = degreesPerSecond;
    }
  }

  void ServoManager::setTarget(size_t servoIndex, int angle)
  {
    _targetAngles[servoIndex] = clampAngle(servoIndex, angle);
    _directions[servoIndex] = 0;
  }

  void ServoManager::setDirection(size_t servoIndex, int direction)
  {
    if (direction > 0)
    {
      _directions[servoIndex] = 1;
    }
    else if (direction < 0)
    {
      _directions[servoIndex] = -1;
    }
    else
    {
      _directions[servoIndex] = 0;
      _targetAngles[servoIndex] = static_cast<int>(lroundf(_currentAngles[servoIndex]));
    }
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
