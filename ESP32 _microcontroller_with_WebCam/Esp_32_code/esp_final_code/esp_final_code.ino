// ------------------- Pin Definitions -------------------
const int ledAlert = 2;    // Built-in LED
const int ledDrowsy = 5;   
const int ledLeft = 18;    
const int ledRight = 19;   

String state = "";  // Stores incoming state

void setup() {
  Serial.begin(115200);

  pinMode(ledAlert, OUTPUT);
  pinMode(ledDrowsy, OUTPUT);
  pinMode(ledLeft, OUTPUT);
  pinMode(ledRight, OUTPUT);

  // Turn off all LEDs initially
  digitalWrite(ledAlert, LOW);
  digitalWrite(ledDrowsy, LOW);
  digitalWrite(ledLeft, LOW);
  digitalWrite(ledRight, LOW);

  Serial.println("✅ ESP32 ready to receive driver state");
}

void loop() {
  // Read serial data if available
  if (Serial.available()) {
    state = Serial.readStringUntil('\n');  // read until newline
    state.trim(); // remove extra whitespace

    // Reset all LEDs
    digitalWrite(ledAlert, LOW);
    digitalWrite(ledDrowsy, LOW);
    digitalWrite(ledLeft, LOW);
    digitalWrite(ledRight, LOW);

    // Turn on the LED corresponding to the state
    if (state == "Alert") {
      digitalWrite(ledAlert, HIGH);
    } 
    else if (state == "Drowsy") {
      digitalWrite(ledDrowsy, HIGH);
    } 
    else if (state == "Left") {
      digitalWrite(ledLeft, HIGH);
    } 
    else if (state == "Right") {
      digitalWrite(ledRight, HIGH);
    }

    // Optional: print the state for debugging
    Serial.print("📡 State received: ");
    Serial.println(state);
  }
}
