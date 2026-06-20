
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// LoRa pins
#define LORA_SCK  18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SS   5
#define LORA_RST  14
#define LORA_DIO0 26

// OLED settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Function to show default Tracking screen
void showTracking() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  display.println("Tracking");
  display.display();
}

void setup() {
  Serial.begin(115200);

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1);
  }

  // Initialize LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed!");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("LoRa Init Failed!");
    display.display();
    while (1);
  }

  // Max-range settings
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);

  Serial.println("LoRa Receiver Ready (Max Range)");

  // Show default Tracking screen
  showTracking();
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String receivedText = "";
    while (LoRa.available()) {
      receivedText += (char)LoRa.read();
    }

    // Serial monitor
    Serial.print("Received: ");
    Serial.println(receivedText);

    // Display received message temporarily
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 16);
    display.println(receivedText);
    display.display();

    // Keep message for 2 seconds
    delay(2000);

    // Return to default Tracking screen
    showTracking();
  }
}