#include "AppConfig.h"

namespace robotarm
{
  const WifiConfig kWifiConfig = {
      "",
      "",
      "RoboticArm-Control",
      "roboticarm",
      15000,
  };

  const ServoConfig kServoConfigs[kServoCount] = {
      {"claw", 14, 90, 0, 180},
      {"base", 12, 0, 0, 180},
      {"shoulder", 27, 0, 0, 180},
      {"elbow", 26, 0, 0, 180},
      {"wrist", 25, 0, 0, 180},
  };
}
