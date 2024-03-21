#include "tank.h"

// Declared externally in defaults.c because cpp doesn't have designated initializers
extern Tank tank;

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
