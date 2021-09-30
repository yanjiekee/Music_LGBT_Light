#include <Arduino.h>
#include <math.h>

#define ANALOG_INPUT_PIN A0

#define WINDOW_WIDTH 100
#define VOLTAGE_RANGE 0.7

int digitalOut = 0;
float analogOut = 0;

void printVoltage(float voltage);

void setup() {
  // Serial Monitor initialization
  Serial.begin(19200);

  analogReference(AR_INTERNAL2V4);
  delay(100);
}

void loop() {
  digitalOut = analogRead(ANALOG_INPUT_PIN);

  analogOut = (digitalOut * 2.4)/1023.00 - 1.59;

  printVoltage(analogOut);
}

void printVoltage(float voltage) {
  float mappedVoltage = voltage + VOLTAGE_RANGE / 2;
  int len = (int)(mappedVoltage * WINDOW_WIDTH / VOLTAGE_RANGE);
  String vPrint = String();
  for (int i = 0; i < WINDOW_WIDTH; i++) {
    if (i == len) vPrint.concat("*");
    else vPrint.concat(" ");
  }

  Serial.println(vPrint);
}
