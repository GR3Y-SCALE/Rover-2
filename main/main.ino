#include <Servo.h>
#include <PinChangeInterrupt.h>

const byte CHANNEL_PIN[] = {10, 11, 12, 13};
volatile unsigned long risingStart[] = {0, 0, 0, 0};
volatile long channelLength[] = {0, 0, 0, 0};

const byte WHEEL_PIN[] = {2,3,4,5}; // 2,3 is right motor.4,5 is left motor
const int WHEEL_TRIM[] = {0,0,0,0} // trim in degrees

const byte ARM_PIN[] = {6,7,8,9}; // 6,7 is right servo. 8,9 is left motor
const int ARM_TRIM[] = {0,0,0,0}

bool debugging = false; // set true for debugging
int deadZone = 200; // deadzone value to allow for less jitter on central stick position and stray motion.     (MIN)--------(_deadzone--CENTRE--deadzone_)--------(MAX)
int channelCentrePosition = 1500; // raw value of centre stick position
int channelBottomPosition = 1000;
int channelTopPosition = 2000;

Servo rightFrontWheel;
Servo rightBackWheel;
Servo leftFrontWheel;
Servo leftBackWheel;

Servo rightFrontArm;
Servo rightBackArm;
Servo leftFrontArm;
Servo leftBackArm;

Servo servoWheel[4] = {rightFrontWheel, rightBackWheel, leftFrontWheel, leftBackWheel};
Servo servoArm[4] = {rightFrontArm, rightBackArm, leftFrontArm, leftBackArm};

void processPin(byte pin) {
  uint8_t trigger = getPinChangeInterruptTrigger(digitalPinToPCINT(CHANNEL_PIN[pin]));

  if(trigger == RISING) {
    risingStart[pin] = micros();
  } else if(trigger == FALLING) {
    channelLength[pin] = micros() - risingStart[pin];
  }
}

void onRising0(void) {
  processPin(0);
}

void onRising1(void) {
  processPin(1);
}

void onRising2(void) {
  processPin(2);
}

void onRising3(void) {
  processPin(3);
}

void setup() {
  Serial.begin(57600);

  pinMode(CHANNEL_PIN[0], INPUT);
  pinMode(CHANNEL_PIN[1], INPUT);
  pinMode(CHANNEL_PIN[2], INPUT);
  pinMode(CHANNEL_PIN[3], INPUT);

  servoWheel[0].attach(WHEEL_PIN[0]);
  servoWheel[1].attach(WHEEL_PIN[1]);
  servoWheel[2].attach(WHEEL_PIN[2]);
  servoWheel[3].attach(WHEEL_PIN[3]);

  servoArm[0].attach(ARM_PIN[0]);
  servoArm[1].attach(ARM_PIN[1]);
  servoArm[2].attach(ARM_PIN[2]);
  servoArm[3].attach(ARM_PIN[3]);

  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(CHANNEL_PIN[0]), onRising0, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(CHANNEL_PIN[1]), onRising1, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(CHANNEL_PIN[2]), onRising2, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(CHANNEL_PIN[3]), onRising3, CHANGE);
}



void serialDebug(long first,long second,long third,long forth) { // will print the values of the channels to serial
  Serial.print(first);
  Serial.print(" | ");
  Serial.print(second);
  Serial.print(" | ");
  Serial.print(third);
  Serial.print(" | ");
  Serial.print(forth);
  Serial.println("");
  delay(100);
}

long getWheelAngle(long position) {
  int servoAngle = 0;
  if (position >= channelCentrePosition + (deadZone / 2)) { // upper most deadzone
    servoAngle = map(position, channelCentrePosition + (deadZone / 2), channelTopPosition, 0, 180);

  } else if (position <= channelCentrePosition - (deadZone / 2)) { // bottom most deadzone
    servoAngle = map(position, channelBottomPosition, channelCentrePosition - (deadZone / 2), 0, 180);

  } else {
    servoAngle = 90; // the value will be within deadzone
  }
  return servoAngle;
}

long getArmAngle(long position) {
  int servoAngle = 0;
  servoAngle = map(position, channelBottomPosition, channelTopPosition, 0, 180);
  return servoAngle;
}

int inverse(int angle) {
  return 180 - angle;
}

void setWheelAngle(long right, long left) {
  for (int i = 0; i < 2; i++) {
    servoWheel[i].write(WHEEL_TRIM[i] + getWheelAngle(right));
  }
  for (int i = 2; i < 4; i++) {
    servoWheel[i].write(WHEEL_TRIM[i] + inverse(getWheelAngle(left)));
  }
}

void setArmAngle(long right,  long left) {
  for (int i = 0; i < 2; i++) {
    if (i % 2 != 0) {
      servoArm[i].write(ARM_TRIM[i] + getArmAngle(right));
    } else {
      servoArm[i].write(ARM_TRIM[i] + inverse(getArmAngle(left)));
    }
  }
  for (int i = 2; i < 4; i++) {
    if (i % 2 != 0) {
      servoArm[i].write(ARM_TRIM[i] + getArmAngle(left));
    } else {
      servoArm[i].write(ARM_TRIM[i] + inverse(getArmAngle(left)));
    }
  }
}


void loop() {
  if (debugging) {
    serialDebug(channelLength[0], channelLength[1], channelLength[2], channelLength[3]);
  }
setArmAngle(channelLength[0], channelLength[2]);
setWheelAngle(channelLength[1], channelLength[3])

}
