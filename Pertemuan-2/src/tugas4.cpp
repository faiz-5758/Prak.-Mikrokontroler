#include <Arduino.h>

const int potensio = 35;
const int merah = 19;
const int kuning = 18;
const int hijau = 5;
const int button = 21;

void setup(){
    Serial.begin(115200);
    pinMode(potensio, INPUT);
    pinMode(merah, OUTPUT);
    pinMode(kuning, OUTPUT);
    pinMode(hijau, OUTPUT);
    pinMode(button, INPUT);
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

    int darurat = digitalRead(button);
    if (darurat == HIGH) {
        digitalWrite(merah, LOW);
        digitalWrite(kuning, LOW);
        digitalWrite(hijau, LOW);
    }
    else {
        if (tegangan <= 1.1) {
            digitalWrite(merah, HIGH);
            digitalWrite(kuning, LOW);
            digitalWrite(hijau, LOW);
        }
        else if (tegangan <= 2.2) {
            digitalWrite(merah, LOW);
            digitalWrite(kuning, HIGH);
            digitalWrite(hijau, LOW); 
        }
        else {
            digitalWrite(merah, LOW);
            digitalWrite(kuning, LOW);
            digitalWrite(hijau, HIGH);
        }
    }

    delay(100);
}