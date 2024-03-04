/*

*/

int in1A = 13;
int in2A = 12;
int enA = 5;

int in1B = 9;
int in2B = 8;
int enB = 6;

int encoderA = 10;
int encoderB = 11;


int speedA = 0, speedB = 0;

void setSpeed(double direction, int speed) {
  speedA = speed*sin(direction*3.14/180.0);
  speedB = speed*cos(direcion*3.14/180.0);
}

/* 
Direction is angle in degrees from North to intended direction
Speed is scalar value from 0 to 255
*/
void driveTank() {
  unsigned long start = millis();
    int countA = 0;
    int prevA = 0;
    
    int countB = 0;
    int prevB = 0;
    
    digitalWrite(in1A, 1);
    digitalWrite(in1B, 1);
    
    unsigned long current = start;
    
    while (current - start < 1000) {
      int nextA = digitalRead(10);
      int nextB = digitalRead(11);
      
      if (nextA != prevA) {
        prevA = nextA;
        countA += prevA;
      }
      if (nextB != prevB) {
        prevB = nextB;
        countB += prevB;
      }
      
      
      current = millis();
    }
    
    Serial.print(countA);
    Serial.print(" ");
    Serial.println(countB);
}

void setup() {
    pinMode(in1A, OUTPUT);
    pinMode(in2A, OUTPUT);
    pinMode(encoderA, INPUT);
    pinMode(enA, OUTPUT);

    pinMode(in1B, OUTPUT);
    pinMode(in2B, OUTPUT);
    pinMode(encoderB, INPUT);
    pinMode(enB, OUTPUT);

    Serial.begin(9600);
    
    setSpeed(100, 0);
}

void loop() {
  
  driveTank();
  
}
