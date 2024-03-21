#include "tank.h"

// Declared externally in defaults.c because cpp doesn't have designated initializers
extern Tank tank;

// Our ISRs will keep track on pin changes on the encoder inputs
// and increment the counter when they change
ISR (PCINT2_vect) {
  tank.motors[B].encoder.count++;
}

ISR (PCINT3_vect) {
  tank.motors[A].encoder.count++;
}

void setup() {
  // Initialize the tank struct (set pinmodes)
  tankInit(&tank);
  Serial.begin(9600);
  
  // Enable interrupts on PORTB
  PCICR = 0b10;
  // Have it trigger when 11 and 10 change
  PCMSK1 |= 0b110;
}

void loop() {
  // Run our joystick drive function in a loop
  tankDriveJoystick(&tank);
}
