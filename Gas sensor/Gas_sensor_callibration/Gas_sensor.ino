// MQ-135 Gas Sensor Example with CO₂ Estimation
const int mq135Pin = A0;        // Analog pin connected to AOUT
const float R0 = 489.3;         // Calibrated R0 in clean air (in kΩ)
const float RL = 10.0;          // Load resistance (in kΩ)

void setup() {
  Serial.begin(9600);
  Serial.println("MQ-135 CO2 Estimation");
}

void loop() {
  int adcValue = analogRead(mq135Pin);
  float voltage = adcValue * (5.0 / 1023.0); // Convert ADC to voltage

  // Calculate Rs (sensor resistance)
  float Rs = ((5.0 - voltage) / voltage) * RL;

  // Calculate ratio of Rs/R0
  float ratio = Rs / R0;

  // Estimate CO₂ ppm (approximate, based on datasheet curve)
  float ppm = 116.6020682 * pow(ratio, -2.769034857);

  // Output values to Serial Monitor
  Serial.print("ADC: ");
  Serial.print(adcValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V | Rs: ");
  Serial.print(Rs, 2);
  Serial.print(" kΩ | Ratio: ");
  Serial.print(ratio, 3);
  Serial.print(" | Estimated CO₂: ");
  Serial.print(ppm, 2);
  Serial.println(" ppm");

  delay(2000); // Delay 2 seconds between readings
}
