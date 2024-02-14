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
	unsigned int speed;
	unsigned int timeToTargetDelay;
	unsigned int acceleration;

} Motor;

Motor m = {11, 10, 7, 6, 0};

inline void motorSetSpeed(Motor* m, int stepsPerSecond) {
	m->speed = stepsPerSecond;
	m->acceleration = 0;
	m->timeToTargetDelay = 0;
}

inline void motorSetSpeed(Motor* m, int stepsPerSecond, int acceleration) {
	m->timeToTargetDelay = abs(stepsPerSecond - m->speed) / (acceleration);
	m->acceleration = stepsPerSecond >= m->speed ? acceleration : -acceleration;
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

	motorSetSpeed(&m, 100);

	Serial.begin(9600);
}

unsigned long curr = 0;

void loop() {
	if (m.speed) {
		for (signed int i = 3; i >= 0; i--) {
			digitalWrite(m.in1A, i == 0 || i == 3);
			digitalWrite(m.in2A, !(i == 0 || i == 3));
			digitalWrite(m.in1B, i < 2);
			digitalWrite(m.in2B, !(i < 2));
			delay(200 / m.speed);
		}
	}
	else {
		digitalWrite(m.in1A, 0);
		digitalWrite(m.in1B, 0);
		digitalWrite(m.in2A, 1);
		digitalWrite(m.in2B, 1);
	}

	if (m.timeToTargetDelay > 0 && curr*1000 <= millis()) {
		m.speed += m.acceleration;
		m.timeToTargetDelay--;
		curr++;
	}

	Serial.println(m.speed);
}
