#include <Arduino.h>

const int mq135Pin = 34;          // ADC pin
const float RL = 10.0;            // Load resistor in kΩ
const int ledPin = 2;             // Built-in LED

float R0 = 10.0;                  // Will be calibrated
const float CO2_THRESHOLD = 1000.0; // ppm threshold

// ---------- Function: calculate Rs ----------
float getRs() {
  int adcValue = analogRead(mq135Pin);
  float voltage = adcValue * (3.3 / 4095.0); 
  if (voltage <= 0.01) return 1e6;
  return ((3.3 - voltage) / voltage) * RL;
}

// ---------- Function: calibrate R0 ----------
float calibrateR0(unsigned long durationMs) {
  Serial.println("Calibrating R0...");
  unsigned long start = millis();
  float sumRs = 0;
  int count = 0;

  while (millis() - start < durationMs) {
    float Rs = getRs();
    sumRs += Rs;
    count++;
    delay(500);
  }

  float avgRs = sumRs / count;
  // force Rs/R0 = 3.6 in clean air → R0 = avgRs / 3.6
  float newR0 = avgRs / 3.6;

  Serial.print("Calibration done. New R0 = ");
  Serial.println(newR0, 2);

  return newR0;
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Calibrate for 30s
  R0 = calibrateR0(30000);
}

void loop() {
  float Rs = getRs();
  float ratio = Rs / R0;

  // Corrected ppm formula, normalized to ~400 ppm in clean air
  float ppm = 400.0 * pow((ratio / 3.6), -2.769);

  Serial.print("Rs: ");
  Serial.print(Rs, 2);
  Serial.print(" kΩ | R0: ");
  Serial.print(R0, 2);
  Serial.print(" kΩ | Ratio: ");
  Serial.print(ratio, 3);
  Serial.print(" | CO₂: ");
  Serial.print(ppm, 2);
  Serial.println(" ppm");

  // LED Alert
  if (ppm >= CO2_THRESHOLD) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  delay(2000);
}
