# AquaFlow

A smart water flow monitoring and pump control system that combines IoT hardware with real-time cloud analytics. AquaFlow tracks water consumption across multiple sensors, detects leakages, and provides a web-based dashboard for complete system control.

**Project by:** Yugesh

## Overview

AquaFlow is a comprehensive water management solution designed to monitor and control water distribution in real-time. The system uses flow sensors to track water usage, calculates costs, detects anomalies, and allows remote pump control through a cloud-connected interface.

![Aquaflow1](https://github.com/user-attachments/assets/72607269-7489-42e1-a6ae-1a0dbaf20fd1)


### Key Features

- **Dual Flow Monitoring:** Real-time tracking of two independent water flow sensors (YF-S401)
- **Remote Pump Control:** ON/OFF control via web dashboard with cloud synchronization
- **Cost Tracking:** Automatic calculation of water usage costs (live, weekly, monthly)
- **Leakage Detection:** Intelligent comparison between sensors to identify irregularities
- **Cloud Integration:** Firebase Realtime Database for seamless data synchronization
- **Responsive Dashboard:** Modern web interface optimized for desktop and mobile
- **Real-time Updates:** Live sensor data and status indicators

## Hardware Requirements

- **Microcontroller:** ESP32 (with WiFi connectivity)
- **Flow Sensors:** 2x YF-S401 water flow sensors
- **Relay Module:** 1x relay (active LOW) for pump control
- **Power Supply:** 5V for ESP32, appropriate voltage for relay and sensors
- **Connecting Components:** Jumper wires, breadboard (optional)

### Pin Configuration (ESP32 GPIO)

| Component | GPIO Pin | Notes |
|-----------|----------|-------|
| Relay Control | GPIO 23 | Active LOW (HIGH = OFF, LOW = ON) |
| Flow Sensor 1 | GPIO 18 | Input with pull-up, FALLING edge interrupt |
| Flow Sensor 2 | GPIO 19 | Input with pull-up, FALLING edge interrupt |

## Software Requirements

- Arduino IDE with ESP32 board support
- Firebase ESP Client library by Mobizt
- Firebase Realtime Database account
- Modern web browser for dashboard access

## Installation & Setup

### 1. Hardware Wiring

Connect components according to the pin configuration:
- Flow sensors: Signal pins to GPIO 18 and 19, GND and 5V to power rails
- Relay module: Control pin to GPIO 23, connect pump circuit in relay's normally open contacts
- ESP32: Power from 5V supply with common ground

### 2. Arduino Code Setup

1. Download the `Arduinocode.cpp` file
2. Open Arduino IDE and create a new sketch
3. Install required library:
   - Go to **Sketch → Include Library → Manage Libraries**
   - Search for "Firebase ESP Client" by Mobizt
   - Install the latest version
4. Replace Firebase and WiFi credentials in the code:
   ```cpp
   #define WIFI_SSID "Your_WiFi_Name"
   #define WIFI_PASSWORD "Your_WiFi_Password"
   #define API_KEY "Your_Firebase_API_Key"
   #define DATABASE_URL "https://your-database-url.firebaseio.com/"
   #define USER_EMAIL "your_email@example.com"
   #define USER_PASSWORD "your_password"
   ```
5. Upload the sketch to your ESP32

### 3. Firebase Setup

1. Create a new Firebase project at [https://console.firebase.google.com](https://console.firebase.google.com)
2. Enable Realtime Database
3. Create the following database structure:
   ```
   /
   ├── pump (string, initial value: "OFF")
   ├── flow1 (number)
   ├── flow2 (number)
   └── data
       ├── totalLiters (number)
       ├── totalPrice (number)
       ├── lastFlow1 (number)
       └── lastFlow2 (number)
   ```
4. Update your Firebase security rules with the provided `firebaserules.txt`

### 4. Web Dashboard Setup

1. Create three files in the same directory:
   - `index.html`
   - `style.css`
   - `script.js`
2. Update Firebase credentials in `script.js`:
   ```javascript
   const firebaseConfig = {
     apiKey: "YOUR_API_KEY_HERE",
     authDomain: "YOUR_PROJECT_ID.firebaseapp.com",
     databaseURL: "https://YOUR_PROJECT_ID-default-rtdb.YOUR_REGION.firebasedatabase.app",
     // ... other credentials
   };
   ```
3. Optional: Add `Aquabg.jpg` background image to the same directory
4. Open `index.html` in a web browser to access the dashboard

## Usage

### Dashboard Controls ###

- **Motor Control Section:** Click ON/OFF buttons to control the pump remotely
- **Sensor Status:** Green indicators show active flow in each sensor
- **Water Usage Cards:** Display current, weekly, and monthly consumption with cost calculations
- **Sensor Flow:** Real-time flow rates in ml/sec for each sensor
- **Leakage Detection:** Green indicator means normal (balanced flow), red indicates potential leakage
- **Reset Button:** Clears accumulated usage data

### Auto Mode

The AUTO pump mode is available in the Arduino code for future automation logic. Currently, it requires custom implementation based on your specific automation requirements.

## Technical Details

### Flow Calculation

- Flow sensors report pulses detected within a 1-second interval
- Conversion formula: **Flow Rate (L/min) = Pulse Count / 7.5**
- Note: The calibration constant (7.5) is specific to YF-S401 sensors; adjust if using different models
- Dashboard converts to ml/sec: **ml/sec = L/min × 1000 / 60**

### Data Transmission

- Arduino reads pump commands from Firebase every loop iteration (~100ms)
- Flow data is sent to Firebase every 1 second
- Flow interrupts are temporarily disabled during data processing to prevent conflicts
- Web dashboard updates in real-time through Firebase listeners

### Leakage Detection Logic

- Compares flow readings from both sensors
- If difference < 0.05 L/min: System displays green (normal)
- If difference ≥ 0.05 L/min: System displays red (potential leakage)
- Useful for detecting pipe breaks or unauthorized usage

## Customization

### Adjusting Pricing

In `script.js`, modify the pricing formula:
```javascript
totalPrice = totalLiters * 0.3; // Change 0.3 to your cost per liter
```

### Changing Flow Calibration

In `Arduinocode.cpp`:
```cpp
float flowRate1 = (pulseCount1 / 7.5); // Change 7.5 to your sensor's calibration
```

### Modifying Leakage Threshold

In `script.js`:
```javascript
const isNormal = Math.abs(lastF1 - lastF2) < 0.05; // Change 0.05 to desired threshold
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| ESP32 won't connect to WiFi | Verify SSID/password are correct; check WiFi signal strength |
| Flow sensors not reading | Verify GPIO pins; check sensor polarity; ensure proper pull-up configuration |
| Firebase connection fails | Confirm API key and database URL; check Firebase security rules allow read/write |
| Dashboard shows no data | Verify Firebase credentials in script.js; check browser console for errors; ensure ESP32 is online |
| Relay not responding | Check active LOW configuration; verify GPIO 23 connection; test relay with simple LED sketch |

## Security Notes

- Current Firebase rules allow open read/write access for development
- **Production Deployment:** Implement proper authentication and restrict database access
- Keep Firebase credentials secure; never commit them to public repositories
- Consider using environment variables for sensitive configuration

## Limitations & Future Improvements

- Current AUTO mode requires custom implementation
- Web dashboard uses in-memory storage; data persists in Firebase only
- No user authentication on dashboard (development setup)
- Leakage detection is threshold-based; advanced ML models could improve accuracy
- Consider adding historical data visualization and alerts

## License

This project is provided as-is for personal and educational use.

## Support & Contact

For questions or contributions, connect with the developer:
- **LinkedIn:** [Yugesh](https://www.linkedin.com/in/yugeshweb)

---

**Last Updated:** 2025
