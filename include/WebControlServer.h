#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include "NetworkManager.h"
#include "ServoManager.h"

namespace robotarm
{
  class WebControlServer
  {
  public:
    WebControlServer(ServoManager &servos, NetworkManager &network, uint16_t port = 80);

    void begin();
    void handleClient();

  private:
    String buildStatusJson() const;
    void handleRoot();
    void handleStatus();
    void handleSet();
    void handleMove();
    void handleSpeed();

    ServoManager &_servos;
    NetworkManager &_network;
    WebServer _server;
  };
}
