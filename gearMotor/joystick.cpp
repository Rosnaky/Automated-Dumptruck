#include <Arduino.h>
#include "joystick.h"

void joystickInit(Joystick* j)
{
  pinMode(j->rxPin, INPUT);
  pinMode(j->ryPin, INPUT);
}
