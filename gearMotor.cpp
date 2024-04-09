#include <Arduino.h>
#include "tank.h"

/**
 * @brief Initialize the tank by setting the pinmodes of the motors and joystick pins 
 *
 * @param t Pointer to the tank struct
 */
void tankInit(Tank* t)
{
  motorInit(t->motors + A);
  motorInit(t->motors + B);
  joystickInit(&t->joystick);
}

/**
 * @brief Will calculate the conversion factor of how cycles can happen at a speed of 255
 *
 * @param t Pointer to the tank struct
 */
void tankCalculateMaxSpeed(Tank* t) {
  unsigned long start = millis();
  
  encoderReset(&t->motors[A].encoder);
  encoderReset(&t->motors[B].encoder);
  motorSetDirection(t->motors + A, CLOCKWISE);
  motorSetDirection(t->motors + B, COUNTER_CLOCKWISE);

  unsigned long current = start;
  
  while (current - start < cycleTime) {
    int nextA = digitalRead(10);
    int nextB = digitalRead(11);
    
    current = millis();
  }
  
  t->maxWheelSpeed = min(t->motors[A].encoder.count, t->motors[B].encoder.count);
  encoderReset(&t->motors[A].encoder);
  encoderReset(&t->motors[B].encoder);
}

/*
 * @brief Direction is angle in degrees from North to intended direction
 *        Speed is scalar value from 0 to 255
 *
 * @param t Pointer to the tank struct
 * @param d direction
 * @param s speed
 */
void tankSetSpeed(Tank* t, double d, int s) {
  t->direction = d;
  t->speed = s;
  int x = s*sin(d*3.14/180.0);
  int y = s*cos(d*3.14/180.0);

  bool index = d > 180;
  t->motors[!index].speed = y;
  t->motors[index].speed = x+y;
}

/*
 * @brief Adjusts speeds to match direction
 * 
 * @param t Pointer to the tank struct
 */
void tankFixSpeeds(Tank* t) {
  int error = 3;
  
  int x = t->speed*sin(t->direction*3.14/180.0);
  int y = t->speed*cos(t->direction*3.14/180.0);
  
  int predicted [2];
  bool index = t->direction < 180;
  predicted[!index] = y/255.0*t->maxWheelSpeed;
  predicted[index] = (x+y)/255.0*t->maxWheelSpeed;
  
  motorCorrectSpeed(t->motors + A, predicted[A], t->maxWheelSpeed, error);
  motorCorrectSpeed(t->motors + B, predicted[B], t->maxWheelSpeed, error);
  
  t->motors[A].speed = max(t->motors[A].speed, -255);
  t->motors[B].speed = max(t->motors[B].speed, -255);
  t->motors[A].speed = min(t->motors[A].speed, 255);
  t->motors[B].speed = min(t->motors[B].speed, 255);
}

/*
 * @brief Drives the tank forward adjusting speeds as needed to maintain a straight line
 *
 * @param t Pointer to the tank struct
 */
void tankDrive(Tank* t) {
  analogWrite(t->motors[A].en, abs(t->motors[A].speed));
  analogWrite(t->motors[B].en, abs(t->motors[B].speed));
  motorSetDirection(t->motors + A, (Direction)(t->motors[A].speed >= 0));
  motorSetDirection(t->motors + B, (Direction)(t->motors[B].speed < 0));
  
  if (millis() - t->lastCycleTime >= cycleTime) {
    t->lastCycleTime = millis();
    tankFixSpeeds(t);
    
    encoderReset(&t->motors[A].encoder);
    encoderReset(&t->motors[B].encoder);
  }
}

/*
 * @brief Runs tankDrive in a loop for the given time
 *
 * @param t Pointer to the tank struct
 */
void tankDriveForTime(Tank* t, double seconds) {
  unsigned long start = millis();
  
  while (millis()-start < seconds*1000) {
    tankDrive(t);
  }
  
  tankSetSpeed(t, 0, 0);
}

/*
 * @brief Turns tank in an arc given parameters
 *
 * @param t Pointer to the tank struct
 * @param degrees is how much you want to turn
 * @param radius is the radius of the turn
 * @param s is the speed of the car while in the turn
 */
void turnTank(Tank* t, int degrees, double radius, int s) {
  
}

/*
 * @brief Turns tank using the output from the joystick
 *
 * @param t Pointer to the tank struct
 */
void tankDriveJoystick(Tank* t) {
  float x = analogRead(t->joystick.rxPin)-512;
  float y = analogRead(t->joystick.ryPin)-512;
  
  // 724 is max hypotenuse length
  int s = sqrt(x*x + y*y)/724*255;
  int d;
  if (x) d = atan(y/x)/3.14*180;
  else d = y > 0 ? 0 : 180;
  
  tankSetSpeed(t, d, s);
  tankDrive(t);
}

void tankDriveRoutine(Tank* t) {
  tankSetSpeed(t, 0, 100);
  tankDriveForTime(t, 1);
  
  tankSetSpeed(t, 30, 100);
  tankDriveForTime(t, 1);
  tankSetSpeed(t, 60, 100);
  tankDriveForTime(t, 1);
  tankSetSpeed(t, 90, 100);
  tankDriveForTime(t, 1);
  
  tankDriveForTime(t, 1);
  // turnTank(270, 100, 0);
}
