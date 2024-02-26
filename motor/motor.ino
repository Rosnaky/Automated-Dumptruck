// Our data structure for a Motor
typedef struct {
	union {                                                  // Using a union with an array so we can also loop through it
		unsigned char pins [6];                              // An array of the 6 pins we need
		struct {                                             // We wrap the pins together in their own anonymouse struct
			unsigned char in1A, in2A, in1B, in2B, enA, enB;  // This gets mapped to the same place in memory as the array
		};
	};
	signed long delay;                                       // Our current delay
	signed long desiredDelay;                                // The delay we want to get to / the delay we are accelerating to
	unsigned long lastTick;                                  // The last time the motor magnets changed their polarity (This is used to time the steps)
	int acceleration;                                        // Our rate of acceleration. (Usually 1)
	unsigned char state;                                     // The current step position we are in (We can use this to determine the next step)
} Motor;

// Sets the delay variables and makes sure acceleration is zero
inline void motorSetDelay(Motor* m, long delay) {
	m->delay = m->desiredDelay = delay;
	m->acceleration = 0;
}

// This is the same as above but actually sets the acceleration (This will be used to speed up)
inline void motorSetDelay(Motor* m, int desiredDelay, int acceleration) {
	m->desiredDelay = desiredDelay;
	m->acceleration = desiredDelay >= m->delay ? acceleration : -acceleration;
}

// Executes one step of the motor
// Will update the last tick, increment the state, change the motor magenets polarities and accelerate if needed
void motorStep(Motor* m) {
	m->lastTick = micros();
	if (m->delay > 0)
		++m->state;
	else if (m->delay < 0)
		--m->state;
	int i = m->state % 4;
	digitalWrite(m->in1A, i == 0 || i == 3);
	digitalWrite(m->in2A, !(i == 0 || i == 3));
	digitalWrite(m->in1B, i < 2);
	digitalWrite(m->in2B, !(i < 2));

	//if (m->delay != m->desiredDelay) 
	//	m->delay += m->acceleration;
}

// Loops through each motor and executes their steps if enough time has passed since the last update
void tankStep(Motor* m, int size) {
	for (int i = 0; i < size; i++)
		if ((micros() - m[i].lastTick) >= abs(m[i].delay))
			motorStep(&m[i]);
}

// Sets all the pins for each motor to output and sets the enable pins to high (As oppose the to analogWrite of 255)
void motorInit(Motor* m, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < 6; j++)
			pinMode(m[i].pins[j], OUTPUT);

		digitalWrite(m[i].enA, HIGH);
		digitalWrite(m[i].enB, HIGH);
	}
}

// Our array of Motor structs
Motor motors[] = {
	{23, 24, 27, 28, 22, 26, 5000},
	{12, 11, 10, 9, 13, 8, 5000},
};

void setup() {
	Serial.begin(9600);
	motorInit(motors, 2);

	motorSetDelay(motors, 5000);
	motorSetDelay(&motors[1], -5000);
}

void loop() {
	tankStep(motors, 2);
}
