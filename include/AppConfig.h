#pragma once

#include <Arduino.h>

namespace robotarm
{
  struct WifiConfig
  {
    const char *stationSsid;
    const char *stationPassword;
    const char *accessPointSsid;
    const char *accessPointPassword;
    unsigned long stationTimeoutMs;
  };

  struct ServoConfig
  {
    const char *name;
    uint8_t pin;
    int initialAngle;
    int minAngle;
    int maxAngle;
  };

  constexpr size_t kServoCount = 5;

  extern const WifiConfig kWifiConfig;
  extern const ServoConfig kServoConfigs[kServoCount];
}
