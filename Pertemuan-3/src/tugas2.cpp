#include <arduino.h>

const int in1 = 35;
const int in2 = 36;
const int pwmPin = 37;
const int stby = 19;

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(stby, OUTPUT);
  
  digitalWrite(stby, HIGH); // Aktifkan driver
  digitalWrite(in1, HIGH);  // Atur arah putaran maju
  digitalWrite(in2, LOW);

  ledcSetup(0, 5000, 8);
  ledcAttachPin(pwmPin, 0);
}

void loop() {
  ledcWrite(0, 200); // Set kecepatan motor (0-255)
}