#include <util/delay.h>
// Right motor
#define enA 12

#define enB 8

// 00 01 11 10

typedef struct {
	union {
		unsigned char pins [4];
		struct {
			unsigned char in1A, in2A, in1B, in2B;
		};
	};
	unsigned int delay;
} Motor;

Motor m = {11, 10, 7, 6, 20};

inline void motorSetSpeed(Motor* m, float stepsPerSecond) {
	m->delay = 360 / stepsPerSecond;
}

ISR(TIMER1_COMPA_vect)
{
   PORTB ^= (1 << 0); // Toggle the LED
}

void setup() {
	Serial.begin(9600);
	pinMode(enA, OUTPUT);
	pinMode(m.in1A, OUTPUT);
	pinMode(m.in2A, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(m.in1B, OUTPUT);
	pinMode(m.in2B, OUTPUT);
	analogWrite(enA, 255);
	analogWrite(enB, 255);
}

void loop() {
	for (signed int i = 3; i >= 0; i--) {
		digitalWrite(m.in1A, i == 0 || i == 3);
		digitalWrite(m.in2A, !(i == 0 || i == 3));
		digitalWrite(m.in1B, i < 2);
		digitalWrite(m.in2B, !(i < 2));
		delay(m.delay);
	}

	/*for (unsigned int i = 0; i < 4; i++) {
		digitalWrite(in1A, i == 2);
		digitalWrite(in2A, !(i == 2));
		digitalWrite(in1B, (i > 1));
		digitalWrite(in2B, !(i > 1));
		delay(delayTime);
	}*/

	/*for (signed char i = 3; i >= 0; i++) {
		digitalWrite(in1A, (i == 0));
		digitalWrite(in2A, !(i == 0));
		digitalWrite(in1B, (i < 2));
		digitalWrite(in2B, !(i < 2));
		delay(delayTime);
	}*/
}
