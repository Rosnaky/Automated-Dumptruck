#include "encoder.h"

void encoderReset(Encoder* e)
{
  e->count = 0;
  e->prev = 0;
}
