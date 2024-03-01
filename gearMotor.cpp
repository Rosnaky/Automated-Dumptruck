/*

*/

void setup() {
    pinMode(13, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(A0, OUTPUT);
    
    pinMode(10, INPUT);

    Serial.begin(9600);
    
    analogWrite(A0, 55);
}

void loop() {
    
    unsigned long start = millis();
    int count = 0;
    int prev = 0;
    
    digitalWrite(13, 1);
    
    unsigned long current = start;
    while (current - start < 1000) {
      int next = digitalRead(10);
      if (next != prev) {
        prev = next;
        count += prev;
      }
      current = millis();
    }
    
    Serial.println(count);
    
}

