#include <Arduino.h>

const int buttonPin = 14;
const int ledPin1 = 27;
const int ledPin2 = 26;
const int ledPin3 = 25;

void setup() {
    pinMode(14, INPUT_PULLDOWN);
    pinMode(27, OUTPUT);
    pinMode(26, OUTPUT);
    pinMode(25, OUTPUT);
}

void loop() {
    if (digitalRead(buttonPin) == HIGH){
        digitalWrite(ledPin1, HIGH);
        digitalWrite(ledPin2, HIGH);
        digitalWrite(ledPin3, HIGH);
    }
    else{
        digitalWrite(ledPin1, LOW);
        digitalWrite(ledPin2, LOW);
        digitalWrite(ledPin3, LOW);
    }
}