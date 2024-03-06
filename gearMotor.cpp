/*

*/

int in1A = 13;
int in2A = 12;
int enA = 5;

int in1B = 9;
int in2B = 8;
int enB = 6;

int encoderA = 11;
int encoderB = 10;


int speedA = 0, speedB = 0;
int direction = 0, speed = 0;
int maxWheelSpeed = 0;

void setSpeed(int d, int s) {
  direction = d;
  speed = s;
  int x = speed*sin(direction*3.14/180.0);
  int y = speed*cos(direction*3.14/180.0);
  
  if (direction < 180) {
    speedA = y;
    speedB = x+y;
  }
  else {
    speedB = y;
    speedA = x+y;
  }
}

void fixSpeeds(int countA, int countB) {
  int error = 3;
  
  int x = speed*sin(direction*3.14/180.0);
  int y = speed*cos(direction*3.14/180.0);
  
  int predictedA, predictedB;
  if (direction < 180) {
    predictedA = y/255.0*maxWheelSpeed;
    predictedB = (x+y)/255.0*maxWheelSpeed;
  }
  else {
    predictedB = y/255*maxWheelSpeed;
    predictedA = (x+y)/255*maxWheelSpeed;
  }
  
  Serial.print(countA);
  Serial.print(" ");
  Serial.print(predictedA);
  Serial.print(" ");
  Serial.print(countB);
  Serial.print(" ");
  Serial.println(predictedB);
  
  if (abs(countA-predictedA) > error) {
    speedA = (countA-predictedA) > 0 ? speedA -= ceil(abs(countA-predictedA)/5.0/maxWheelSpeed*255.0) :
    speedA += ceil(abs(countA-predictedA)/5.0/maxWheelSpeed*255.0);
  }
  if (abs(countB-predictedB) > error) {
    speedB = (countB-predictedB) > 0 ? speedB -= ceil(abs(countB-predictedB)/5.0/maxWheelSpeed*255.0) :
    speedB += ceil(abs(countB-predictedB)/5.0/maxWheelSpeed*255.0);
  }
  
  
  speedA = max(speedA, -255);
  speedB = max(speedB, -255);
  speedA = min(speedA, 255);
  speedB = min(speedB, 255);
  
  Serial.print(speedA);
  Serial.print(" ");
  Serial.println(speedB);
  Serial.println("");
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
  
  analogWrite(enA, abs(speedA));
  analogWrite(enB, abs(speedB));
  if (speedA > 0) {
    digitalWrite(in1A, 1);
    digitalWrite(in2A, 0);
  }
  else {
    digitalWrite(in1A, 0);
    digitalWrite(in2A, 1);
  }
  
  if (speedB > 0) {
    digitalWrite(in1B, 0);
    digitalWrite(in2B, 1);
  }
  else {
    digitalWrite(in1B, 1);
    digitalWrite(in2B, 0);
  }
  
  unsigned long current = start;
  
  while (current - start < 1000) {
    int nextA = digitalRead(encoderA);
    int nextB = digitalRead(encoderB);
    
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
  
  fixSpeeds(countA, countB);
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
    
  unsigned long start = millis();
  int countA = 0;
  int prevA = 0;
  
  int countB = 0;
  int prevB = 0;
  
  analogWrite(enA, 255);
  analogWrite(enB, 255);
  digitalWrite(in1A, 1);
  digitalWrite(in2A, 0);
  digitalWrite(in1B, 0);
  digitalWrite(in2B, 1);

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
  
  maxWheelSpeed = min(countA, countB);
  Serial.println(maxWheelSpeed);
    
    
  setSpeed(0, 100);
}

void loop() {
  
  driveTank();
  
}
