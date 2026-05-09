#include "NetworkManager.h"

#include <WiFi.h>

namespace robotarm
{
  void NetworkManager::begin(const WifiConfig &config)
  {
    _config = &config;
    _accessPointMode = false;

    WiFi.mode(WIFI_STA);
    if (strlen(config.stationSsid) > 0)
    {
      WiFi.begin(config.stationSsid, config.stationPassword);
      Serial.printf("Connecting to %s", config.stationSsid);

      const unsigned long started = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - started < config.stationTimeoutMs)
      {
        delay(400);
        Serial.print(".");
      }

      Serial.println();
      if (WiFi.status() == WL_CONNECTED)
      {
        return;
      }
    }

    WiFi.disconnect(true);
    delay(200);
    WiFi.mode(WIFI_AP);
    _accessPointMode = true;
    WiFi.softAP(config.accessPointSsid, config.accessPointPassword);
  }

  bool NetworkManager::isAccessPointMode() const
  {
    return _accessPointMode;
  }

  String NetworkManager::activeNetworkName() const
  {
    if (_config == nullptr)
    {
      return "";
    }

    return _accessPointMode ? String(_config->accessPointSsid) : String(_config->stationSsid);
  }

  String NetworkManager::activeIpAddress() const
  {
    return _accessPointMode ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
  }
}
