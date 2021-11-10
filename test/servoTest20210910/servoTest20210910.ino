#include <Servo.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;
Servo servo7;
Servo servo8;

int pins[8] = {2,3,4,5,6,7,8,9};

Servo wheel[4] = {servo1, servo2, servo3, servo4};
Servo arm[4] = {servo5, servo6, servo7, servo8};

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 4; i++) {
    wheel[i].attach(pins[i]);
  }
  for (int j = 0; j < 4; j++) {
    arm[j].attach(pins[j + 4]);
  }

}


void loop() {
  // put your main code here, to run repeatedly:

}
