```cpp
#include <esp_now.h> 
#include <WiFi.h> 
uint8_t receiverMAC[] = {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0xEF}; 
int ledState = 0; 
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) { 
Serial.print("Send Status: "); 
Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail"); 
} 
void sendData(int value) { 
esp_now_send(receiverMAC, (uint8_t *)&value, sizeof(value)); 
} 
void setup() { 
Serial.begin(115200); 
WiFi.mode(WIFI_STA); 
if (esp_now_init() != ESP_OK) { 
Serial.println("Error initializing ESP-NOW"); 
return; 
} 
esp_now_register_send_cb(OnDataSent); 
esp_now_peer_info_t peerInfo; 
memcpy(peerInfo.peer_addr, receiverMAC, 6); 
peerInfo.channel = 0; 
peerInfo.encrypt = false; 
if (esp_now_add_peer(&peerInfo) != ESP_OK) { 
Serial.println("Failed to add peer"); 
return; 
} 
} 
void loop() { 
ledState = !ledState; // Toggle 0/1 
sendData(ledState); 
Serial.print("Sent: "); 
Serial.println(ledState); 
delay(1000); 
} 
Alright — I’ll break it down **line-by-line** so you know exactly what each part does and 
why it’s there. ---
```
### **Code Walkthrough** 
```cpp 
#include <esp_now.h>  
``` 
* Includes the **ESP-NOW library** — needed for sending and receiving data wirelessly 
between ESP32 boards without Wi-Fi network. 
```cpp 
#include <WiFi.h> 
``` 
* Includes the **WiFi library** — even though we aren’t using the internet, we still use it to 
set Wi-Fi mode (`WIFI_STA`) which ESP-NOW requires. --- 
```cpp 
uint8_t receiverMAC[] = {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0xEF}; 
``` 
* This array stores the **MAC address of the receiving ESP32**. 
* `uint8_t` = unsigned 8-bit integer (0–255). 
* MAC addresses are 6 bytes long and uniquely identify a device. --- 
```cpp 
int ledState = 0; 
``` 
* Variable that will hold the LED state (`0` for OFF, `1` for ON) that we want to send. --- 
```cpp 
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) { 
Serial.print("Send Status: "); 
Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail"); 
} 
``` 
* **Callback function** — ESP-NOW automatically calls this after sending data. 
* `mac_addr` → MAC of the device we sent data to (not used here). 
* `status` → tells if sending was successful or failed. 
* Prints `"Success"` if sent correctly, `"Fail"` if not. --- 
```cpp 
void sendData(int value) { 
esp_now_send(receiverMAC, (uint8_t *)&value, sizeof(value)); 
} 
``` 
* This function sends **one integer value** using ESP-NOW. 
* `receiverMAC` → where to send the data. 
* `(uint8_t *)&value` → takes the memory address of `value` and treats it as a byte array 
(ESP-NOW sends bytes, not integers directly). 
* `sizeof(value)` → tells ESP-NOW how many bytes to send (an `int` is usually 4 bytes on 
ESP32). --- 
```cpp 
void setup() { 
Serial.begin(115200); 
``` 
* Starts the **Serial Monitor** at 115200 baud so we can print debug messages. 
```cpp 
WiFi.mode(WIFI_STA); 
``` 
* Sets the ESP32 to **station mode** (like a laptop connecting to Wi-Fi). 
* ESP-NOW only works in `WIFI_STA` or `WIFI_AP_STA` mode. --- 
```cpp 
if (esp_now_init() != ESP_OK) { 
Serial.println("Error initializing ESP-NOW"); 
return; 
} 
``` 
* Initializes ESP-NOW. 
* If it fails (`!= ESP_OK`), print an error and stop running `setup()` using `return`. --- 
```cpp 
esp_now_register_send_cb(OnDataSent); 
``` 
* Registers the `OnDataSent` function as the **send callback** so it’s called whenever 
data is sent. --- 
```cpp 
esp_now_peer_info_t peerInfo; 
``` 
* Creates a `peerInfo` structure — holds details about the device we’re going to send data 
to. 
```cpp 
memcpy(peerInfo.peer_addr, receiverMAC, 6); 
``` 
* Copies the receiver’s MAC address into `peerInfo.peer_addr`. 
* `memcpy` is used because we’re copying an **array**. 
```cpp 
peerInfo.channel = 0; 
``` 
* Sets the Wi-Fi channel to `0`. This means “use whatever channel the ESP-NOW network 
chooses”. 
```cpp 
peerInfo.encrypt = false; 
``` 
* Disables encryption for simplicity (ESP-NOW supports encryption with shared keys, but 
not used here). --- 
```cpp 
if (esp_now_add_peer(&peerInfo) != ESP_OK) { 
Serial.println("Failed to add peer"); 
return; 
} 
``` 
* Adds the receiver as a **peer** so the ESP32 knows where to send data. 
* If adding fails, prints an error and stops `setup()`. --- 
```cpp 
void loop() { 
ledState = !ledState; // Toggle 0/1 
``` 
* Changes `ledState` from `0` → `1` → `0` → `1` on every loop run. 
* This simulates ON/OFF switching without a button. 
```cpp 
sendData(ledState); 
``` 
* Calls our custom function to send the `ledState` to the receiver. 
```cpp 
Serial.print("Sent: "); 
Serial.println(ledState); 
``` 
* Prints the sent value to Serial Monitor for debugging. 
```cpp 
delay(1000); 
} 
```Got it — let’s say we want to send **temperature data** from one ESP32 to another 
using **ESP-NOW**. 
We don’t need to use a `struct` if we’re just sending a single value like temperature, but I’ll 
show you both ways. --- 
## **   
Without Struct (just a float value)** 
**Sender ESP32:** 
```cpp 
#include <esp_now.h> 
#include <WiFi.h> 
uint8_t receiverMAC[] = {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0xEF}; // Receiver's MAC address 
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) { 
Serial.print("Send Status: "); 
Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail"); 
} 
void sendTemperature(float temp) { 
esp_now_send(receiverMAC, (uint8_t *)&temp, sizeof(temp)); 
} 
void setup() { 
Serial.begin(115200); 
WiFi.mode(WIFI_STA); 
if (esp_now_init() != ESP_OK) { 
Serial.println("Error initializing ESP-NOW"); 
return; 
} 
esp_now_register_send_cb(OnDataSent); 
esp_now_peer_info_t peerInfo; 
memcpy(peerInfo.peer_addr, receiverMAC, 6); 
peerInfo.channel = 0; 
peerInfo.encrypt = false; 
if (esp_now_add_peer(&peerInfo) != ESP_OK) { 
Serial.println("Failed to add peer"); 
return; 
} 
} 
void loop() { 
f
 loat temperature = 25.7; // Example value, you can replace with sensor reading 
sendTemperature(temperature); 
Serial.print("Sent Temperature: "); 
Serial.println(temperature); 
delay(2000); 
} 
``` 
**Receiver ESP32:** 
```cpp 
#include <esp_now.h> 
#include <WiFi.h> 
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) { 
f
 loat temp; 
memcpy(&temp, incomingData, sizeof(temp)); 
Serial.print("Received Temperature: "); 
Serial.println(temp); 
} 
void setup() { 
Serial.begin(115200); 
WiFi.mode(WIFI_STA); 
if (esp_now_init() != ESP_OK) { 
Serial.println("Error initializing ESP-NOW"); 
return; 
} 
esp_now_register_recv_cb(OnDataRecv); 
} 
void loop() { 
// Nothing here, reception happens via callback 
} 
``` --- 
## **   
With Struct (better if you have more than one variable)** 
If later you also want humidity, pressure, etc.: 
```cpp 
typedef struct { 
f
 loat temperature; 
f
 loat humidity; 
} SensorData; 
``` 
Yes — with **ESP-NOW** on ESP32, you can send **any data type** — integer, float, 
string, char, even an entire struct — as long as you convert it to bytes before sending, 
because `esp_now_send()` only transmits raw byte arrays (`uint8_t *`). 
Here’s the key: 
```cpp 
esp_err_t esp_now_send(const uint8_t *peer_addr, const uint8_t *data, size_t len); 
``` 
* `peer_addr` → MAC address of the receiver 
* `data` → pointer to your data in byte form 
* `len` → number of bytes to send --- 
##    
Sending a **character** 
```cpp 
char letter = 'A'; 
esp_now_send(receiverMAC, (uint8_t *)&letter, sizeof(letter)); 
``` 
**Receiver:** 
```cpp 
char letter; 
memcpy(&letter, incomingData, sizeof(letter)); 
Serial.println(letter); 
``` --- 
##    
Sending a **string** 
Since ESP-NOW needs fixed sizes, small strings are easiest to send as `char[]`. 
**Sender:** 
```cpp 
char message[] = "Hello ESP32"; 
esp_now_send(receiverMAC, (uint8_t *)message, sizeof(message));  
``` 
(`sizeof(message)` ensures the null terminator `\0` is sent too.) 
**Receiver:** 
```cpp 
char message[50]; 
memcpy(message, incomingData, len); 
message[len] = '\0'; // Ensure null-termination 
Serial.println(message); 
``` --- 
##    
Sending an **integer** 
```cpp 
int number = 12345; 
esp_now_send(receiverMAC, (uint8_t *)&number, sizeof(number)); 
``` 
**Receiver:** 
```cpp 
int number; 
memcpy(&number, incomingData, sizeof(number)); 
Serial.println(number); 
``` --- 
If you mix different data types (e.g., string + float + int), you should use a **struct** so 
you send them together and in the right order. 
--- 
If you want, I can give you a **single example where ESP32 sends an integer, a character, 
and a string at the same time** without needing a struct, so you see how byte packing 
works. That would show the difference clearly. 
well formated pdf of this  
. 