#ifndef JOYSTICK_H
#define JOYSTICK_H

typedef struct {
  int rxPin, ryPin;
} Joystick;

void joystickInit(Joystick* j);

#endif /* JOYSTICK_H */
