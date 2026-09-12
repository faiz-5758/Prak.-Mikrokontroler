#include <Arduino.h>

const int potensio = 35;

void setup() {
    Serial.begin(115200);
    pinMode(potensio, INPUT);
}

void loop() {
    int adc = analogRead(potensio);
    float tegangan = (adc / 4095.0) * 3.3;
    float hambatan = (adc / 4095.0) * 10000.0;

    Serial.print("ADC: ");
    Serial.print(adc);
    Serial.print(", Tegangan: ");
    Serial.print(tegangan);
    Serial.print(", Hambatan: ");
    Serial.println(hambatan);

    delay(500);
}