#ifndef MOTOR_H
#define MOTOR_H

#include <math.h>
#include "encoder.h"

typedef struct {
  Encoder encoder;
  int in1, in2, en, speed;
} Motor;

typedef enum {
  CLOCKWISE,
  COUNTER_CLOCKWISE,
  MAX_DIRECTION,
} Direction;

/*
 * @brief Initialize motor pin modes
 *
 * @param m Pointer to the Motor struct
 */
void motorInit(Motor* m);

/*
 * @brief Sets the spin direction of the motor
 *
 * @param m Pointer to the Motor struct
 * @param d Direction enum (either CLOCKWISE or COUNTER_CLOCKWISE)
 */
void motorSetDirection(Motor* m, Direction d);

/*
 * @brief Corrects speed to make the motor follow a straight line
 *
 * @param m Pointer to the Motor struct
 * @param predicted the predicted amount of cycles
 * @param maxWheelSpeed the maximum wheel speed
 * @param error the error threshold
 */
void motorCorrectSpeed(Motor* m, int predicted, int maxWheelSpeed, int error);

#endif /* MOTOR_H */
