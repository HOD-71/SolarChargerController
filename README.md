```markdown
# ☀️ SolarChargerController (IIoT-Enabled Smart Solar MPPT/PWM Controller)

![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![FreeRTOS](https://img.shields.io/badge/FreeRTOS-v10.0-green.svg)
![ESP32](https://img.shields.io/badge/Hardware-ESP32-red.svg)
![Docker](https://img.shields.io/badge/Infrastructure-Docker%20%7C%20Mosquitto-blue)
![Python](https://img.shields.io/badge/Client-Python%203.x-yellow)

An industrial-grade, multi-core solar charge controller firmware designed for **ESP32** utilizing **FreeRTOS** for concurrent task management, hardware-level thread safety via **Mutexes**, and **Finite State Machine (FSM)** charge logic. Telemetry is streamed over **MQTT/JSON** to a containerized Mosquitto broker for real-time monitoring and analytics.

---
## 🏗️ System Architecture

The firmware decouples sensor acquisition and network telemetry across ESP32's dual cores to guarantee zero latency in critical control loops.

```text
+-----------------------------------------------------------------+
|                          ESP32 Microcontroller                  |
|                                                                 |
|   +--------------------------+     +------------------------+   |
|   |   Core 0: ReadSensors    |     |  Core 1: MqttPublish   |   |
|   |  - ADC Sampling (200ms)  |     | - MQTT JSON Streaming  |   |
|   |  - FSM Battery Charge    |     | - Network Auto-Reconnect|  |
|   +------------+-------------+     +-----------+------------+   |
|                |                               |                |
|                +-------> [ FreeRTOS Mutex ] <--+                |
|                               |                                 |
+-------------------------------+---------------------------------+
                                |
                           Wi-Fi / MQTT
                                |
                                v
                +-------------------------------+
                | Docker Container: Mosquitto   |
                +---------------+---------------+
                                |
                                v
                +-------------------------------+
                | Python Telemetry Consumer     |
                | (Real-time Analytics & Logs)  |
                +-------------------------------+

---
---
## ✨ Key Features

- **Concurrent Multi-Core Architecture:** Task pinning on ESP32 cores using FreeRTOS (`xTaskCreatePinnedToCore`).
- **Thread-Safe Telemetry:** Access protection using FreeRTOS Mutexes (`xSemaphoreTake` / `xSemaphoreGive`).
- **Finite State Machine (FSM):** Structured battery charging stages (`BULK`, `ABSORPTION`, `FLOAT`, `FAULT`).
- **Standardized IIoT Streaming:** JSON payload formatting using `ArduinoJson v7` sent to MQTT broker.
- **Containerized Infrastructure:** Pre-configured Docker Compose environment for Eclipse Mosquitto MQTT Broker.
- **Python Monitoring Client:** Real-time data subscriber featuring structured logging and state decoding.

---

## 🛠️ Project Structure

```text
SolarChargerController/
├── include/              # Header files and configurations
├── src/
│   └── main.cpp          # Main firmware code with FreeRTOS tasks & FSM
├── docker/
│   ├── docker-compose.yml# Container orchestration for Mosquitto Broker
│   └── mosquitto.conf    # MQTT Broker security & access control
├── scripts/
│   ├── monitor.py        # Python telemetry consumer client
│   └── requirements.txt  # Python environment dependencies
├── platformio.ini        # PlatformIO environment & dependency definitions
└── README.md             # Project documentation

```

---

## 🚀 Getting Started

### Prerequisites

1. **PlatformIO IDE** (VS Code Extension)
2. **Docker Desktop** installed and running
3. **Python 3.8+**

### 1. Launch MQTT Broker Infrastructure

Navigate to the `docker/` directory and spin up the Mosquitto container:

```bash
cd docker
docker-compose up -d

```

### 2. Build & Flash Firmware

Open the project in PlatformIO, adjust your Wi-Fi & MQTT credentials in `src/main.cpp`, and upload to your ESP32:

```bash
pio run --target upload

```

### 3. Run Telemetry Monitor

Install Python dependencies and run the monitoring script:

```bash
cd scripts
pip install -r requirements.txt
python monitor.py

```

---

## 🗺️ Roadmap & Future Enhancements

To bring this prototype to full production capability, the following features are planned:

* [ ] **Hardware Watchdog Timer (WDT):** Integration for self-healing against severe EMI/NVR lockups.
* [ ] **ADC Calibration & Noise Filtering:** Software-level moving average filter to offset ESP32 ADC non-linearity.
* [ ] **Non-Volatile Storage (NVS):** Storing Wi-Fi credentials and state logs across system restarts.
* [ ] **Grafana Dashboard:** Visualizing long-term telemetry metrics over InfluxDB/Prometheus.

---

## 📜 License

Distributed under the MIT License. See `LICENSE` for more information.

```

```