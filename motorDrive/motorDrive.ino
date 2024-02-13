#include <util/delay.h>
// Right motor
#define enA 12
#define in1A 11 
#define in2A 10

#define enB 8
#define in1B 7 
#define in2B 6

#define delayTime 20

// 00 01 11 10

void setup() {
	Serial.begin(9600);
	pinMode(enA, OUTPUT);
	pinMode(in1A, OUTPUT);
	pinMode(in2A, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(in1B, OUTPUT);
	pinMode(in2B, OUTPUT);
	analogWrite(enA, 255);
	analogWrite(enB, 255);
}

void loop() {
	/*for (unsigned int i = 0; i < 4; i++) {
		digitalWrite(in1A, i == 0);
		digitalWrite(in2A, !(i == 0));
		digitalWrite(in1B, i < 2);
		digitalWrite(in2B, !(i < 2));
		delay(delayTime);
	}*/

	for (unsigned int i = 0; i < 4; i++) {
		digitalWrite(in1A, i == 2);
		digitalWrite(in2A, !(i == 2));
		digitalWrite(in1B, (i > 1));
		digitalWrite(in2B, !(i > 1));
		delay(delayTime);
	}
}
