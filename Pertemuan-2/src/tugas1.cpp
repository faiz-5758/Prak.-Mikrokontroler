#include <Arduino.h>

const int ledPin = 16;

void setup() {
  pinMode(16, OUTPUT);
}

void loop() {
  delay(3000);
  digitalWrite(16, HIGH);
  delay(3000);
  digitalWrite(16, LOW);
}
