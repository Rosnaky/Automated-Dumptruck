#ifndef TANK_H
#define TANK_H

#include "motor.h"
#include "joystick.h"

typedef struct {
  Motor* motors;
  Joystick joystick;
  double direction;
  int speed;
  int lastCycleTime;
  int maxWheelSpeed;
} Tank;

/**
 * @brief Initialize the tank by setting the pinmodes of the motors and joystick pins 
 *
 * @param t Pointer to the tank struct
 */
void tankInit(Tank* t);

/**
 * @brief Will calculate the conversion factor of how cycles can happen at a speed of 255
 *
 * @param t Pointer to the tank struct
 */
void tankCalculateMaxSpeed(Tank* t);

/*
 * @brief Direction is angle in degrees from North to intended direction
 *        Speed is scalar value from 0 to 255
 *
 * @param t Pointer to the tank struct
 * @param d direction
 * @param s speed
 */
void tankSetSpeed(Tank* t, double d, int s);

/*
 * @brief Adjusts speeds to match direction
 * 
 * @param t Pointer to the tank struct
 */
void tankFixSpeeds(Tank* t);

/*
 * @brief Drives the tank forward adjusting speeds as needed to maintain a straight line
 *
 * @param t Pointer to the tank struct
 */
void tankDrive(Tank* t);

/*
 * @brief Runs tankDrive in a loop for the given time
 *
 * @param t Pointer to the tank struct
 */
void tankDriveForTime(Tank* t, double seconds);

/*
 * @brief Turns tank in an arc given parameters
 *
 * @param t Pointer to the tank struct
 * @param degrees is how much you want to turn
 * @param radius is the radius of the turn
 * @param s is the speed of the car while in the turn
 */
void tankDriveJoystick(Tank* t);

/*
 * @brief Turns tank using the output from the joystick
 *
 * @param t Pointer to the tank struct
 */
void tankDriveRoutine(Tank* t);

#define A 0
#define B 1

#endif /* TANK_H */
