#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;
void setup() {
Serial.begin(115200);
ina219.begin();
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

delay(1000);
}