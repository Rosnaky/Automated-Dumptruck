/*

*/
#include <math.h>

int in1B = 13;
int in2B = 12;
int enB = 5;

int in1A = 9;
int in2A = 8;
int enA = 6;

int encoderA = 11;
int encoderB = 10;


int speedA = 0, speedB = 0;
double direction = 0;
int speed = 0;
int maxWheelSpeed = 100;

const int numSlits = 20;

int rxPin = A0;
int ryPin = A1;


void getMaxWheelSpeed() {
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
}

/* 
Direction is angle in degrees from North to intended direction
Speed is scalar value from 0 to 255
*/
void setSpeed(double d, int s) {
  direction = d;
  speed = s;
  int x = s*sin(d*3.14/180.0);
  int y = s*cos(d*3.14/180.0);
  Serial.print(x);
  Serial.print(" ");
  Serial.println(y);
  
  if (d > 180) {
    speedA = y;
    speedB = x+y;
    // speedB = sqrt((x*x)+(y*y));
    // if (x < 0) speedB *= -1;
  }
  else {
    speedB = y;
    speedA = x+y;
    // speedA = sqrt((x*x)+(y*y));
    // if (x < 0) speedA *= -1;
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

void driveTank() {
  unsigned long start = millis();
  int countA = 0;
  int prevA = 0;
  
  int countB = 0;
  int prevB = 0;
  
  analogWrite(enA, abs(speedA));
  analogWrite(enB, abs(speedB));
  if (speedA < 0) {
    digitalWrite(in1A, 1);
    digitalWrite(in2A, 0);
  }
  else {
    digitalWrite(in1A, 0);
    digitalWrite(in2A, 1);
  }
  
  if (speedB < 0) {
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

void driveTankForTime(double seconds) {
  unsigned long start = millis();
  
  while (millis()-start < seconds*1000) {
    driveTank();
  }
  
  setSpeed(0, 0);
  driveTank();
}

/*
degrees is how much you want to turn
radius is the radius of the turn
s is the speed of the car while in the turn
*/
void turnTank(int degrees, double radius, int s) {
  
}


void driveJoystick() {
  float x = analogRead(rxPin)-512;
  float y = analogRead(ryPin)-512;
  
  // 724 is max hypotenuse length
  int s = sqrt(x*x + y*y)/724*255;
  int d = atan(x/y)/3.14*180;
  
  // Serial.print(x);
  // Serial.print(" ");
  // Serial.print(y);
  // Serial.print(" ");
  // Serial.print(s);
  // Serial.print(" ");
  // Serial.println(d);
  
  setSpeed(d, s);
  driveTank();
  
}

void driveRoutine() {
  setSpeed(0, 100);
  driveTankForTime(1);
  
  setSpeed(30, 100);
  driveTankForTime(1);
  setSpeed(60, 100);
  driveTankForTime(1);
  setSpeed(90, 100);
  driveTankForTime(1);
  
  // setSpeed(120, 100);
  // driveTankForTime(1);
  // setSpeed(150, 100);
  // driveTankForTime(1);
  // setSpeed(180, 100);
  
  // setSpeed(210, 100);
  // driveTankForTime(1);
  // setSpeed(240, 100);
  // driveTankForTime(1);
  // setSpeed(270, 100);
  // driveTankForTime(1);
  
  // setSpeed(300, 100);
  // driveTankForTime(1);
  // setSpeed(330, 100);
  // driveTankForTime(1);
  // setSpeed(360, 100);
  
  
  driveTankForTime(1);
  // turnTank(270, 100, 0);
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
  
  pinMode(rxPin, INPUT);
  pinMode(ryPin, INPUT);

  Serial.begin(9600);
   
  
  // driveRoutine();
}

void loop() {
  
  driveJoystick();
  // driveTank();
  
}
