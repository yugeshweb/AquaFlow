/*
AquaFlow - By Yugesh
  
This code connects an ESP32 to Firebase and monitors
two water flow sensors (YF-S401) along with a relay 
for pump control. Data (flow1 & flow2) is sent to Firebase
every second, and pump commands (ON/OFF/AUTO) are received
from the database in real time.

Before running this code:

1. Replace Wi-Fi and Firebase credentials with your own.
2. Ensure your Firebase Realtime Database structure (contains nodes: /pump, /flow1, /flow2)
3. Connect components according to the pin config below.
*/

#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"

// Wi-Fi Configuration – CHANGE THESE VALUES

#define WIFI_SSID "Your_WiFi_Name"         // Replace with your Wi-Fi name
#define WIFI_PASSWORD "Your_WiFi_Password" // Replace with your Wi-Fi password

// Firebase Configuration – CHANGE THESE VALUES

#define API_KEY "Your_Firebase_API_Key"                          // Get from Firebase project settings
#define DATABASE_URL "https://your-database-url.firebaseio.com/" // Your Firebase RTDB URL
#define USER_EMAIL "your_email@example.com"                      // Must be a registered Firebase user
#define USER_PASSWORD "your_password"                            // Corresponding password


// Firebase Objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Pin Configuration (ESP32 GPIO pins)
// You can change these if your wiring differs.
const int relayPin = 23;      // Relay control pin (Active LOW)
const int flowSensor1 = 18;   // Flow sensor 1 input pin (YF-S401)
const int flowSensor2 = 19;   // Flow sensor 2 input pin (YF-S401)

// Flow Measurement Variables
volatile int pulseCount1 = 0;
volatile int pulseCount2 = 0;
unsigned long lastSendTime = 0;

// Interrupt Service Routines for Flow Sensors
void IRAM_ATTR pulseCounter1() 
{ 
pulseCount1++; 
}
void IRAM_ATTR pulseCounter2() 
{ 
pulseCount2++; 
}

// Setup Function
void setup() 
{
Serial.begin(115200);

// Relay setup
pinMode(relayPin, OUTPUT);
digitalWrite(relayPin, HIGH); // Relay OFF initially (active LOW)

// Flow sensor setup
pinMode(flowSensor1, INPUT_PULLUP);
pinMode(flowSensor2, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(flowSensor1), pulseCounter1, FALLING);
attachInterrupt(digitalPinToInterrupt(flowSensor2), pulseCounter2, FALLING);

// Wi-Fi connection
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("Connecting to Wi-Fi");
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(300);
}
Serial.println("\nWi-Fi Connected");

// Firebase setup
config.api_key = API_KEY;
config.database_url = DATABASE_URL;
auth.user.email = USER_EMAIL;
auth.user.password = USER_PASSWORD;
config.token_status_callback = tokenStatusCallback;

Firebase.begin(&config, &auth);
Firebase.reconnectWiFi(true);
Serial.println("Firebase Initialized");
}

// Loop Function
void loop() 
{
if (Firebase.ready()) {

// Read pump status from Firebase
if (Firebase.RTDB.getString(&fbdo, "/pump")) {  // Path: /pump (do not change unless needed)
String command = fbdo.to<String>();
Serial.print("Firebase command: ");
Serial.println(command);

if (command == "ON") 
{
digitalWrite(relayPin, LOW);   // Turn ON pump
} 
else if (command == "OFF") 
{
digitalWrite(relayPin, HIGH);  // Turn OFF pump
}
else if (command == "AUTO") {
// Optional: Add automation logic based on sensor data
}
} else 
{
Serial.print("Failed to read pump: ");
Serial.println(fbdo.errorReason());
}

// Send flow data every 1 second
if (millis() - lastSendTime > 1000) {
detachInterrupt(digitalPinToInterrupt(flowSensor1));
detachInterrupt(digitalPinToInterrupt(flowSensor2));

// Convert pulse counts to flow rate (L/min)
float flowRate1 = (pulseCount1 / 7.5);
float flowRate2 = (pulseCount2 / 7.5);

pulseCount1 = 0;
pulseCount2 = 0;
lastSendTime = millis();

attachInterrupt(digitalPinToInterrupt(flowSensor1), pulseCounter1, FALLING);
attachInterrupt(digitalPinToInterrupt(flowSensor2), pulseCounter2, FALLING);
Serial.printf("Flow1: %.2f L/min | Flow2: %.2f L/min\n", flowRate1, flowRate2);

// Send data to Firebase
bool success1 = Firebase.RTDB.setFloat(&fbdo, "/flow1", flowRate1); // Path: /flow1
if (success1) 
{
Serial.println("flow1 sent to Firebase");
} 
else 
{
Serial.print("flow1 failed: ");
Serial.println(fbdo.errorReason());
}
bool success2 = Firebase.RTDB.setFloat(&fbdo, "/flow2", flowRate2); // Path: /flow2
if (success2) 
{
Serial.println("flow2 sent to Firebase");
} 
else
{
Serial.print("flow2 failed: ");
Serial.println(fbdo.errorReason());
}
}
}
delay(100); // Keep loop responsive for accurate flow measurement
}

/* 
Notes:
- Wi-Fi and Firebase credentials must be updated before upload.
- Ensure you’ve installed the “Firebase ESP Client” library by Mobizt.
- Flow sensor calibration constant (7.5) is for YF-S401; adjust if using a different model.
- Database paths (/pump, /flow1, /flow2) should exist in your Firebase RTDB.
*/
