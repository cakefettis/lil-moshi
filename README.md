# 🐣 MochiBot — ESP32 Emotional Companion

A small companion robot inspired by the **Dasai Mochi**, built with an **ESP32**, an **OLED display**, and **MQTT-based notifications**.

## ✨ Features
- Displays a face that changes with emotion (`happy`, `sad`, `sleepy`, etc.)
- Reacts to **touch**
- Shows **text notifications** via MQTT
- Simple **desktop Python publisher** to send messages and moods

## 🧩 Hardware
| Component | Description |
|------------|-------------|
| ESP32 Dev Board | Main MCU |
| 0.96" OLED (SSD1306) | Display for face |
| TTP223 Touch Sensor | For pokes/touch |
| Optional RGB LED | For status/mood color |

## ⚙️ Firmware
Firmware is in `src/mochi_face.ino`.  
It connects to Wi-Fi and subscribes to three MQTT topics:
- `mochi/notify`
- `mochi/mood`
- `mochi/command`

## 💻 MQTT Control
You can use any MQTT publisher (e.g. `mosquitto_pub`) or your own Python script.

Example commands:
```bash
mosquitto_pub -t "mochi/mood" -m "happy"
mosquitto_pub -t "mochi/notify" -m "Time for tea!"
mosquitto_pub -t "mochi/command" -m "sleep"
```

## 🧠 Future Ideas
- Add servos for head movement
- Add sound / speech feedback
- Integrate Google Maps ETA updates

## 🪪 License
MIT License
