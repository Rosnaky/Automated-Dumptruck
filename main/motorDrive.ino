/*

*/
// Right motor
#define enA 12
#define in1A 11 
#define in2A 10

#define enB 8
#define in1B 7 
#define in2B 6

#define delayTime 20

void setup() {
  Serial.begin(9600);
	pinMode(enA, OUTPUT);
	pinMode(in1A, OUTPUT);
	pinMode(in2A, OUTPUT);
	pinMode(enB, OUTPUT);
	pinMode(in1B, OUTPUT);
	pinMode(in2B, OUTPUT);
}

void loop() {
  analogWrite(enA, 255);
  digitalWrite(in1A, HIGH);
  digitalWrite(in2A, LOW);
  
  analogWrite(enB, 255);
  digitalWrite(in1B, HIGH);
  digitalWrite(in2B, LOW);
  delay(delayTime);

  analogWrite(enA, 255);
  digitalWrite(in1A, LOW);
  digitalWrite(in2A, HIGH);
  
  analogWrite(enB, 255);
  digitalWrite(in1B, HIGH);
  digitalWrite(in2B, LOW);
  delay(delayTime);

  analogWrite(enA, 255);
  digitalWrite(in1A, LOW);
  digitalWrite(in2A, HIGH);
  
  analogWrite(enB, 255);
  digitalWrite(in1B, LOW);
  digitalWrite(in2B, HIGH);
  delay(delayTime);

  analogWrite(enA, 255);
  digitalWrite(in1A, HIGH);
  digitalWrite(in2A, LOW);
  
  analogWrite(enB, 255);
  digitalWrite(in1B, LOW);
  digitalWrite(in2B, HIGH);
  delay(delayTime);
}

void setMotor(int en, int in1, int in2, int speed) {
	digitalWrite(in1, speed < 0);
	digitalWrite(in2, speed > 0);

	analogWrite(en, abs(speed));
}
