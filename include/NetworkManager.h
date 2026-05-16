#pragma once

#include <Arduino.h>

#include "AppConfig.h"

namespace robotarm
{
  class NetworkManager
  {
  public:
    void begin(const WifiConfig &config);

    bool isAccessPointMode() const;
    String activeNetworkName() const;
    String activeIpAddress() const;

  private:
    const WifiConfig *_config = nullptr;
    bool _accessPointMode = false;
  };
}
