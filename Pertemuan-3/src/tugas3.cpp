#include <Arduino.h>

const int in1 = 35;
const int in2 = 36;
const int pwmPin = 37;
const int stby = 19;
const int potPin = 45;

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
  int potValue = analogRead(potPin); // Hasil: 0 - 4095
  int pwmValue = map(potValue, 0, 4095, 0, 255); // Konversi ke 0 - 255
  ledcWrite(0, pwmValue);
}

