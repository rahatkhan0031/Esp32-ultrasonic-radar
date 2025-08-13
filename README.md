# 🛰️ ESP32 Ultrasonic Radar (HC-SR04 + SG90 Servo)

Simple radar “scanner” built with **ESP32**, **HC-SR04** ultrasonic sensor, and **9g micro servo**.  
The servo sweeps the sensor; distances are measured and printed to Serial (and can be graphed).

---

## 🧰 Components
- ESP32 Dev Board (3.3V logic)
- HC-SR04 Ultrasonic Distance Sensor
- 9g Micro Servo (Tower Pro SG90 or similar)
- External 5V supply for servo (recommended)
- Breadboard + wires

---

## 🔌 Wiring (ESP32)
> **Important:** HC-SR04’s **ECHO** pin is 5V. Use a **voltage divider/level shifter** to bring it to 3.3V for the ESP32.

| Module | Pin | ESP32 Pin | Notes |
|---|---|---|---|
| HC-SR04 | VCC | 5V | (from USB/5V rail) |
| HC-SR04 | GND | GND | common ground |
| HC-SR04 | TRIG | GPIO 5 | any output-capable pin |
| HC-SR04 | ECHO | GPIO 18 | **Level shift to 3.3V** |
| Servo | V+ | 5V | use external 5V; not from ESP32 5V if jitter |
| Servo | GND | GND | common ground |
| Servo | Signal | GPIO 12 | PWM-capable pin |

> You can change pins in the sketch if you use different ones.

---

## 🖼️ Diagram
![Wiring diagram](esp32_radar_wiring.jpg)

---

## 🧪 Libraries
- **ESP32Servo** (by Kevin Harrington / Michael Margolis)  
  `Arduino IDE → Tools → Manage Libraries… → search "ESP32Servo" → Install`
- (Optional) **NewPing** is not required; the sketch uses `pulseIn()`.

---

## ▶️ How to Run
1. Install the **ESP32 Arduino core** in Boards Manager; select your ESP32 board.
2. Install **ESP32Servo** (see above).
3. Open `sketch_radar_system_using_Ultrasonic_sensor.ino`.
4. Adjust pin numbers at the top if needed.
5. Upload to ESP32.
6. Open Serial Monitor (e.g., **115200 baud**) to see angles & distances.

---

## ⚠️ Power Tips
- Servos cause brownouts. Power the **servo from a separate 5V supply** and tie grounds together.
- Level-shift **ECHO → ESP32** to **3.3V** (two resistors: 1.8k + 3.3k ≈ 3.3V).

---

## 📜 License
MIT
