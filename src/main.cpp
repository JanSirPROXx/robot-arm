#include <ESP32Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

void setup() {
  servo1.attach(14); //claw
  servo2.attach(12);
  servo3.attach(25);
  servo4.attach(26);
  servo5.attach(27);

  servo1.write(90);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
  servo5.write(0);
}

void loop() {
  delay(6000);
  // servo1.write(180);
  // servo2.write(180);
  // servo3.write(180);
  // servo4.write(180);
  // servo5.write(180);

  delay(2000);
  // servo1.write(0);
  // servo2.write(0);
  // servo3.write(0);
  // servo4.write(0);
  // servo5.write(0);
}