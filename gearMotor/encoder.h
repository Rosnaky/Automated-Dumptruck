#ifndef ENCODER_H
#define ENCODER_H

typedef struct {
  int pin;
  int count;
  int prev;
} Encoder;

/* Encoder constants */
#define numSlits  20
#define cycleTime 1000

void encoderReset(Encoder* e);

#endif /* ENCODER_H */
