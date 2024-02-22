typedef struct {
	union {
		unsigned char pins [6];
		struct {
			unsigned char in1A, in2A, in1B, in2B, enA, enB;
		};
	};
	long delay;
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
	m->lastTick = micros();
	if (m->delay > 0) {
		int i = (--(m->state)+3)%4;
		digitalWrite(m->in1A, i == 0 || i == 3);
		digitalWrite(m->in2A, !(i == 0 || i == 3));
		digitalWrite(m->in1B, i < 2);
		digitalWrite(m->in2B, !(i < 2));
	}
	else if (m->delay < 0) {
		int i = ++(m->state) % 4;
		digitalWrite(m->in1A, i == 0 || i == 3);
		digitalWrite(m->in2A, !(i == 0 || i == 3));
		digitalWrite(m->in1B, i < 2);
		digitalWrite(m->in2B, !(i < 2));
	}

	/*if (m->timeToTargetDelay > 0 && curr*1000 <= micros()) {
		m->speed += m->acceleration;
		m->timeToTargetDelay--;
		curr++;
	}
	else if (curr*1000 <= micros()) curr++;*/
}

void tankDrive(Motor* m, int size)
{
	for (int i = 0; i < size; i++)
	{
		if ((micros() - m[i].lastTick) >= (m[i].delay))
			motorDrive(&m[i]);
	}
}

void motorInit(Motor* m, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < 6; j++)
			pinMode(m[i].pins[j], OUTPUT);

		digitalWrite(m->enA, HIGH);
		digitalWrite(m->enB, HIGH);
	}
}

Motor motors[] = {
	{23, 24, 27, 28, 22, 26, 5000},
	{11, 10, 7, 6, 12, 8, 5000}
};

void setup() {
	Serial.begin(9600);
	motorInit(motors, 2);

	motorSetSpeed(&motors[0], 100);
	motorSetSpeed(&motors[1], -100);

	pinMode(11, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(8, OUTPUT);

	digitalWrite(12, HIGH);
	digitalWrite(8, HIGH);
}

void loop() {
  tankDrive(motors, 2);
}
