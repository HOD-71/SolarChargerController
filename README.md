Markdown
# ☀️ SolarChargerController (IIoT-Enabled Smart Solar MPPT/PWM Controller)

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-v10.0-green.svg)
![ESP32](https://img.shields.io/badge/Hardware-ESP32-red.svg)
![Docker](https://img.shields.io/badge/Infrastructure-Docker%20%7C%20Mosquitto-blue)
![Python](https://img.shields.io/badge/Client-Python%203.x-yellow)
![License](https://img.shields.io/badge/License-MIT-green.svg)

An industrial-grade, multi-core solar charge controller firmware designed for **ESP32** utilizing **FreeRTOS** for concurrent task management, hardware-level thread safety via **Mutexes**, and **Finite State Machine (FSM)** charge logic. Telemetry is streamed over **MQTT/JSON** to a containerized Mosquitto broker for real-time monitoring and analytics.

> 📌 **Legacy Note:** Looking for the initial basic Proteus simulation & Arduino prototype? Check out the [`v1.0-legacy`](https://github.com/HOD-71/SolarChargerController/tree/v1.0-legacy) branch.

---

## 🏗️ System Architecture

The firmware decouples sensor acquisition and network telemetry across ESP32's dual cores to guarantee zero latency in critical control loops.

```mermaid
graph TD
    subgraph ESP32 ["ESP32 Microcontroller"]
        direction TB
        subgraph Core0 ["Core 0: ReadSensors"]
            A[ADC Sampling 200ms] --> B[FSM Battery Charge Logic]
        end
        subgraph Core1 ["Core 1: MqttPublish"]
            C[MQTT JSON Streaming] --> D[Network Auto-Reconnect]
        end
        Core0 <-->|FreeRTOS Mutex| M((Shared State))
        Core1 <-->|FreeRTOS Mutex| M
    end

    ESP32 -->|Wi-Fi / MQTT JSON| Docker["Docker Container: Mosquitto Broker"]
    Docker --> Python["Python Telemetry Consumer (Analytics & Logs)"]
✨ Key Features
Concurrent Multi-Core Architecture: Task pinning on ESP32 cores using FreeRTOS (xTaskCreatePinnedToCore).

Thread-Safe Telemetry: Access protection using FreeRTOS Mutexes (xSemaphoreTake / xSemaphoreGive).

Finite State Machine (FSM): Structured battery charging stages (BULK, ABSORPTION, FLOAT, FAULT).

Standardized IIoT Streaming: JSON payload formatting using ArduinoJson v7 sent to MQTT broker.

Containerized Infrastructure: Docker-ready Mosquitto MQTT Broker integration.

Python Monitoring Client: Real-time data subscriber featuring structured logging and state decoding.

🛠️ Project Structure
Plaintext
SolarChargerController/
├── firmware/
│   ├── include/          # Header files and configurations
│   ├── src/
│   │   └── main.cpp      # Main firmware code with FreeRTOS tasks & FSM
│   └── platformio.ini    # PlatformIO environment & dependency definitions
├── scripts/
│   ├── monitor.py        # Python telemetry consumer client
│   └── requirements.txt  # Python environment dependencies
├── .gitignore            # Git ignore rules
├── LICENSE               # MIT License
└── README.md             # Project documentation
🚀 Getting Started
Prerequisites
PlatformIO IDE (VS Code Extension)

Docker Desktop installed and running

Python 3.8+

1. Launch MQTT Broker Infrastructure
Run Eclipse Mosquitto using Docker:

Bash
docker run -d --name mqtt-broker -p 1883:1883 eclipse-mosquitto
2. Build & Flash Firmware
Navigate to the firmware/ directory, update Wi-Fi & MQTT credentials in src/main.cpp, and flash your ESP32:

Bash
cd firmware
pio run --target upload
3. Run Telemetry Monitor
Install Python dependencies and execute the monitoring script:

Bash
cd scripts
pip install -r requirements.txt
python monitor.py
🗺️ Roadmap & Future Enhancements
[ ] Hardware Watchdog Timer (WDT): Integration for self-healing against severe EMI lockups.

[ ] ADC Calibration & Filtering: Software moving average filter to offset ESP32 ADC non-linearity.

[ ] Non-Volatile Storage (NVS): Storing Wi-Fi credentials and state logs across system restarts.

[ ] Grafana Dashboard: Visualizing long-term telemetry metrics over InfluxDB/Prometheus.

📜 License
Distributed under the MIT License. See LICENSE for more information.