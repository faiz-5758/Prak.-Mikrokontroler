#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUZZER_PIN 26

Adafruit_INA219 ina219;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
Serial.begin(115200);
ina219.begin();
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
display.clearDisplay();
display.setTextSize(1);
display.setTextColor(SSD1306_WHITE);
pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
float arus = ina219.getCurrent_mA();
float tegangan_V = ina219.getBusVoltage_V();
float daya_mW = ina219.getPower_mW();

Serial.print("Arus: ");
Serial.print(arus);
Serial.print(" mA\tTegangan: ");
Serial.print(tegangan_V);
Serial.print(" V\tDaya: ");
Serial.print(daya_mW);
Serial.println(" mW");

display.clearDisplay();
display.setCursor(0, 0);      

display.println("MONITOR INA219");
if(arus > 500.0) {
    display.println("Status: OVERLOAD");
    digitalWrite(BUZZER_PIN, HIGH);
} else {
    display.println("Status: NORMAL");
    digitalWrite(BUZZER_PIN, LOW);
}

display.print("Tegangan : ");
display.print(tegangan_V);
display.println(" V");

display.print("Arus     : ");
display.print(arus);
display.println(" mA");

display.print("Daya     : ");
display.print(daya_mW);
display.println(" mW");

display.display();

delay(1000);
}