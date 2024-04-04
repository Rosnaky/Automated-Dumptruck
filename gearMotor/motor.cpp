#include <Arduino.h>
#include "motor.h"

/*
 * @brief Initialize motor pin modes
 *
 * @param m Pointer to the Motor struct
 */
void motorInit(Motor* m)
{
  pinMode(m->in1, OUTPUT);
  pinMode(m->in2, OUTPUT);
  pinMode(m->en, OUTPUT);
  pinMode(m->encoder.pin, INPUT);
}

/*
 * @brief Sets the spin direction of the motor
 *
 * @param m Pointer to the Motor struct
 * @param d Direction enum (either CLOCKWISE or COUNTER_CLOCKWISE)
 */
void motorSetDirection(Motor* m, Direction d)
{
  digitalWrite(m->in1, !d);
  digitalWrite(m->in2, d);
}

/*
 * @brief Corrects speed to make the motor follow a straight line
 *
 * @param m Pointer to the Motor struct
 * @param predicted the predicted amount of cycles
 * @param maxWheelSpeed the maximum wheel speed
 * @param error the error threshold
 */
void motorCorrectSpeed(Motor* m, int predicted, int maxWheelSpeed, int error)
{
  if (m->encoder.count-predicted > error)
    return;
  if (m->encoder.count-predicted > 0)
    m->speed -= ceil(abs(m->encoder.count-predicted)/5.0/maxWheelSpeed*255.0);
  else
    m->speed += ceil(abs(m->encoder.count-predicted)/5.0/maxWheelSpeed*255.0);
}
