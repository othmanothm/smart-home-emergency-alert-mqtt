# Smart Home Emergency Alert System Using MQTT

An IoT-based emergency alert system using ESP32, MQTT, Python, and Telegram Bot API.
The system detects motion and abnormal temperature conditions, activates a local alarm, publishes emergency events through MQTT, and forwards critical alerts to Telegram in real time.

## Overview

This project demonstrates a complete IoT monitoring and alerting workflow:

1. ESP32 reads sensor data from a PIR motion sensor and DHT22 temperature sensor.
2. If motion is detected or temperature reaches the emergency threshold, the ESP32 activates a local alarm using an LED and buzzer.
3. The ESP32 publishes emergency messages to an MQTT broker.
4. A Python MQTT subscriber bridge receives the alert message.
5. The Python bridge forwards the alert to a Telegram bot as a real-time notification.

The system was implemented and tested using Wokwi simulation, VS Code, PlatformIO, MQTT, Python, and Telegram Bot API.

## Features

* Motion detection using a PIR sensor
* Temperature monitoring using a DHT22 sensor
* Emergency threshold detection for high temperature
* Local alarm using LED and buzzer
* MQTT publish/subscribe communication
* Python MQTT-to-Telegram bridge
* Real-time Telegram emergency notifications
* Wokwi simulation support
* PlatformIO project structure
* Local alarm still works even if MQTT or internet connectivity is unavailable

## Tech Stack

* ESP32
* Arduino Framework
* PlatformIO
* Wokwi Simulator
* MQTT
* Python
* paho-mqtt
* Telegram Bot API
* PIR Motion Sensor
* DHT22 Temperature Sensor
* LED
* Buzzer

## System Architecture

The project is divided into three main layers:

### 1. Embedded Sensing Layer

The ESP32 reads sensor values and applies emergency detection logic.

Components:

* ESP32
* PIR motion sensor
* DHT22 temperature sensor
* LED
* Buzzer

### 2. Communication Layer

MQTT is used as the main communication protocol.
The ESP32 works as an MQTT publisher and sends messages to specific MQTT topics.

### 3. Notification Layer

A Python subscriber listens to the emergency MQTT topic and forwards received alerts to Telegram using the Telegram Bot API.

## MQTT Topics

```text
othman/home/security/motion
othman/home/environment/temperature
othman/home/emergency/alert
othman/home/system/status
```

| Topic                                 | Purpose                            |
| ------------------------------------- | ---------------------------------- |
| `othman/home/security/motion`         | Motion detection messages          |
| `othman/home/environment/temperature` | Temperature readings               |
| `othman/home/emergency/alert`         | Critical emergency alerts          |
| `othman/home/system/status`           | System startup and status messages |

## Hardware Connections

| Device                | ESP32 Pin | Function                 |
| --------------------- | --------: | ------------------------ |
| PIR Motion Sensor OUT |   GPIO 27 | Digital motion input     |
| DHT22 DATA            |   GPIO 15 | Temperature sensor input |
| Buzzer                |   GPIO 26 | Audio alarm output       |
| Red LED               |   GPIO 25 | Visual alarm output      |

## Emergency Logic

| Condition           | System Response                                                     |
| ------------------- | ------------------------------------------------------------------- |
| Motion detected     | LED and buzzer turn ON for 5 seconds, then MQTT alert is published  |
| Temperature >= 40°C | LED and buzzer stay ON, then overtemperature alert is published     |
| Temperature < 40°C  | Temperature alarm turns OFF unless motion alarm is active           |
| MQTT offline        | Local alarm still works, but remote notification may be unavailable |

## Project Structure

```text
smart-home-emergency-alert-mqtt/
├── include/
├── lib/
├── src/
│   └── main.cpp
├── telegram_bridge/
│   ├── mqtt_to_telegram.py
│   ├── requirements.txt
│   └── .env.example
├── test/
├── docs/
│   └── Smart_Home_Emergency_Alert_System_Report.pdf
├── diagram.json
├── platformio.ini
├── wokwi.toml
├── README.md
├── LICENSE
└── .gitignore
```

## Environment Variables

The real `.env` file must not be committed to GitHub.

Create a local file:

```text
telegram_bridge/.env
```

Use this template:

```env
BOT_TOKEN=your_telegram_bot_token
CHAT_ID=your_telegram_chat_id
MQTT_BROKER=broker.hivemq.com
MQTT_PORT=1883
MQTT_TOPIC=othman/home/emergency/alert
```

## How to Run

### 1. Run the ESP32 Firmware

Open the project in VS Code with PlatformIO installed.

Build and upload/simulate the firmware:

```bash
pio run
```

Run the Wokwi simulation using the included:

```text
diagram.json
wokwi.toml
```

### 2. Run the Python Telegram Bridge

Go to the Python bridge directory:

```bash
cd telegram_bridge
```

Install dependencies:

```bash
pip install -r requirements.txt
```

Create your local `.env` file using the template above.

Run the bridge:

```bash
python mqtt_to_telegram.py
```

### 3. Test the System

In Wokwi:

* Trigger the PIR motion sensor to test motion alerts.
* Increase the DHT22 temperature to 40°C or higher to test overtemperature alerts.
* Check the Serial Monitor for system logs.
* Check Telegram for real-time alert messages.

## Security Notes

This project was implemented for educational IoT practice. The current version uses a public MQTT broker for testing and simulation.

Known security limitations:

* Public MQTT broker usage
* No MQTT authentication
* No TLS encryption
* MQTT topics can be guessed if not protected
* Telegram bot token must be protected and never committed to GitHub

Suggested security improvements:

* Use a private MQTT broker
* Enable MQTT username/password authentication
* Enable TLS encryption
* Apply topic-based access control
* Store secrets only in environment variables
* Add rate limiting or duplicate-alert filtering in the Python bridge

## Project Report

The full academic project report is available here:

[Smart Home Emergency Alert System Report](docs/Smart_Home_Emergency_Alert_System_Report.pdf)

## Author

Othman Othman
Computer Engineering Student
Al-Quds University

## License

This project is licensed under the MIT License.
