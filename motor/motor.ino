#include <util/delay.h>
// Right motor
#define enA 12

#define enB 8

// 00 01 11 10

typedef struct {
	union {
		unsigned char pins [6];
		struct {
			unsigned char in1A, in2A, in1B, in2B, en1A, en1B;
		};
	};
	int speed;
	unsigned int timeToTargetDelay;
	int acceleration;
	unsigned int state;
	unsigned long lastTick;

} Motor;

inline void motorSetSpeed(Motor* m, int stepsPerSecond) {
	m->speed = stepsPerSecond;
	m->acceleration = 0;
	m->timeToTargetDelay = 0;
}

inline void motorSetSpeed(Motor* m, int stepsPerSecond, int acceleration) {
	m->timeToTargetDelay = abs(stepsPerSecond - m->speed) / (acceleration);
	m->acceleration = stepsPerSecond >= m->speed ? acceleration : -acceleration;
}

unsigned long curr = 0;

void motorDrive(Motor* m) {
	m->lastTick = millis();
	if (m->speed > 0) {
    
//		int i = --m->state % 4;
    int i = m->state;
    m->state = (m->state+3)%4;
		digitalWrite(m->in1A, i == 0 || i == 3);
		digitalWrite(m->in2A, !(i == 0 || i == 3));
		digitalWrite(m->in1B, i < 2);
		digitalWrite(m->in2B, !(i < 2));
	}
	else if (m->speed < 0) {
		int i = ++m->state % 4;
		digitalWrite(m->in1A, i == 0 || i == 3);
		digitalWrite(m->in2A, !(i == 0 || i == 3));
		digitalWrite(m->in1B, i < 2);
		digitalWrite(m->in2B, !(i < 2));
	}

	if (m->timeToTargetDelay > 0 && curr*1000 <= millis()) {
		m->speed += m->acceleration;
		m->timeToTargetDelay--;
		curr++;
	}
	else if (curr*1000 <= millis()) curr++;
}

void tankDrive(Motor* m, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (m->speed == 0) {
			digitalWrite(m->in1A, 0);
			digitalWrite(m->in1B, 0);
			digitalWrite(m->in2A, 1);
			digitalWrite(m->in2B, 1);
		}
		else if ((millis() - m[i].lastTick) >= (200 / m->speed))
			motorDrive(m + i);
	}
}

void motorInit(Motor* m, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < 6; j++)
			pinMode(m->pins[j], OUTPUT);
	}
	digitalWrite(m->en1A, HIGH);
	digitalWrite(m->en1B, HIGH);
}

Motor motors[] = {
	{11, 10, 7, 6, 12, 8},
	{23, 24, 27, 28, 22, 26}
};

void setup() {
	Serial.begin(9600);
	motorInit(motors, 2);

	motorSetSpeed(motors, 100);
	
	Serial.begin(9600);
}

void loop() {
  
  tankDrive(motors, 2);
 
}
