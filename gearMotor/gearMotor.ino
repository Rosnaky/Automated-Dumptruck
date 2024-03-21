#include "tank.h"

Motor motors [] = {
  {
    .encoder = { .pin = 11 },
    .in1 = 9,
    .in2 = 8,
    .en = 6,
  },
  {
    .encoder = { .pin = 10 },
    .in1 = 13,
    .in2 = 12,
    .en = 5,
  },
};

Tank tank = {
  .motors = motors,
  .joystick = { A0, A1 },
  .maxWheelSpeed = 100,
}

ISR (PCINT2_vect) {
  tank.motors[B].encoder.count++;
}
ISR (PCINT3_vect) {
  tank.motors[A].encoder.count++;
}

void setup() {
  tankInit(&tank);
  Serial.begin(9600);
  
  PCICR = 0b10;
  PCMSK1 |= 0b110;
}

void loop() {
  tankDriveJoystick(&tank);
}
