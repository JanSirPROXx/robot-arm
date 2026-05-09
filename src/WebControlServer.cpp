#include "WebControlServer.h"

#include "WebPage.h"

namespace robotarm
{
  namespace
  {
    String jsonString(const String &str)
    {
      String out = "\"";
      for (size_t i = 0; i < str.length(); ++i)
      {
        const char c = str.charAt(i);
        if (c == '\\' || c == '"')
        {
          out += '\\';
        }
        out += c;
      }
      out += "\"";
      return out;
    }
  }

  WebControlServer::WebControlServer(ServoManager &servos, NetworkManager &network, uint16_t port)
      : _servos(servos), _network(network), _server(port)
  {
  }

  void WebControlServer::begin()
  {
    _server.on("/", [this]() { handleRoot(); });
    _server.on("/status", [this]() { handleStatus(); });
    _server.on("/set", [this]() { handleSet(); });
    _server.begin();
  }

  void WebControlServer::handleClient()
  {
    _server.handleClient();
  }

  String WebControlServer::buildStatusJson() const
  {
    String json = "{";
    json += "\"mode\":";
    json += jsonString(_network.isAccessPointMode() ? "AP" : "WiFi");
    json += ",\"network\":";
    json += jsonString(_network.activeNetworkName());
    json += ",\"ip\":";
    json += jsonString(_network.activeIpAddress());
    json += ",\"servos\":[";

    for (size_t i = 0; i < _servos.count(); ++i)
    {
      const ServoConfig &servo = _servos.config(i);
      if (i > 0)
      {
        json += ",";
      }
      json += "{";
      json += "\"id\":";
      json += String(i);
      json += ",\"name\":";
      json += jsonString(servo.name);
      json += ",\"pin\":";
      json += String(servo.pin);
      json += ",\"angle\":";
      json += String(_servos.angle(i));
      json += ",\"minAngle\":";
      json += String(servo.minAngle);
      json += ",\"maxAngle\":";
      json += String(servo.maxAngle);
      json += "}";
    }

    json += "]}";
    return json;
  }

  void WebControlServer::handleRoot()
  {
    _server.send(200, "text/html", makeControlPage());
  }

  void WebControlServer::handleStatus()
  {
    _server.send(200, "application/json", buildStatusJson());
  }

  void WebControlServer::handleSet()
  {
    if (!_server.hasArg("servo") || !_server.hasArg("angle"))
    {
      _server.send(400, "application/json", "{\"error\":\"Missing servo or angle\"}");
      return;
    }

    const long servoIndex = _server.arg("servo").toInt();
    const long angle = _server.arg("angle").toInt();
    if (servoIndex < 0 || !_servos.isValidIndex(static_cast<size_t>(servoIndex)))
    {
      _server.send(404, "application/json", "{\"error\":\"Servo not found\"}");
      return;
    }

    _servos.setAngle(static_cast<size_t>(servoIndex), static_cast<int>(angle));
    _server.send(200, "application/json", buildStatusJson());
  }
}
