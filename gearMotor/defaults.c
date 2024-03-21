#include "tank.h"

Motor motors [] = {
  // Motor A
  {
    .encoder = { .pin = 11 },
    .in1 = 9,
    .in2 = 8,
    .en = 6,
  },
  // Motor B
  {
    .encoder = { .pin = 10 },
    .in1 = 13,
    .in2 = 12,
    .en = 5,
  },
};

#define A0 14
#define A1 15

Tank tank = {
  .motors = motors,
  .joystick = { A0, A1 },
  .maxWheelSpeed = 100,
};
