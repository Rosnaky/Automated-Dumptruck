typedef struct {
	union {
		unsigned char pins [6];
		struct {
			unsigned char in1A, in2A, in1B, in2B, enA, enB;
		};
	};
	long delay;
	long desiredDelay;
	unsigned long lastTick;
	int acceleration;
	unsigned char state;
} Motor;

inline void motorSetDelay(Motor* m, long delay) {
	m->delay = m->desiredDelay = delay;
	m->acceleration = 0;
}

inline void motorSetDelay(Motor* m, int desiredDelay, int acceleration) {
	m->desiredDelay = desiredDelay;
	m->acceleration = desiredDelay >= m->delay ? acceleration : -acceleration;
}

void motorDrive(Motor* m) {
	m->lastTick = micros();
	if (m->delay > 0)
		++m->state %= 4;
	else if (m->delay < 0)
		--m->state %= 4;
	digitalWrite(m->in1A, m->state == 0 || m->state == 3);
	digitalWrite(m->in2A, !(m->state == 0 || m->state == 3));
	digitalWrite(m->in1B, m->state < 2);
	digitalWrite(m->in2B, !(m->state < 2));

	if (m->delay != m->desiredDelay) 
		m->delay += m->acceleration;
}

void tankDrive(Motor* m, int size) {
	for (int i = 0; i < size; i++)
		if ((micros() - m[i].lastTick) >= (m[i].delay))
			motorDrive(&m[i]);
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

	motorSetDelay(motors, 100);
	motorSetDelay(motors + 1, -100);
}

void loop() {
	tankDrive(motors, 2);
}
