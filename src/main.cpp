#include <Arduino.h>

#include "AppConfig.h"
#include "NetworkManager.h"
#include "ServoManager.h"
#include "WebControlServer.h"

namespace
{
  robotarm::ServoManager servoManager;
  robotarm::NetworkManager networkManager;
  robotarm::WebControlServer controlServer(servoManager, networkManager);
}

void setup()
{
  Serial.begin(115200);
  delay(500);

  servoManager.begin();
  networkManager.begin(robotarm::kWifiConfig);
  controlServer.begin();

  Serial.println();
  Serial.printf("Mode: %s\n", networkManager.isAccessPointMode() ? "AP" : "WiFi");
  Serial.printf("Network: %s\n", networkManager.activeNetworkName().c_str());
  Serial.printf("Control URL: http://%s\n", networkManager.activeIpAddress().c_str());
}

void loop()
{
  servoManager.update();
  controlServer.handleClient();
}
