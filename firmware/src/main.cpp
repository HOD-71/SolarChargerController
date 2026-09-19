#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi & MQTT Broker Configuration
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER = "192.168.1.100"; // Docker Mosquitto IP
const int   MQTT_PORT   = 1883;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// System State Machine Definition
enum ChargingState {
    BULK,
    ABSORPTION,
    FLOAT,
    FAULT
};

ChargingState currentState = BULK;

// System Telemetry Data Struct
struct TelemetryData {
    float solarVoltage;
    float batteryVoltage;
    float chargeCurrent;
    ChargingState state;
};

TelemetryData systemData = {0.0f, 0.0f, 0.0f, BULK};
SemaphoreHandle_t dataMutex;

// FreeRTOS Task Prototypes
void TaskReadSensors(void *pvParameters);
void TaskMqttPublish(void *pvParameters);

void setupWiFi() {
    delay(10);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void reconnectMQTT() {
    while (!mqttClient.connected()) {
        if (mqttClient.connect("SolarCharger_ESP32")) {
            // Connected to MQTT broker
        } else {
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}

void setup() {
    Serial.begin(115200);
    dataMutex = xSemaphoreCreateMutex();

    setupWiFi();
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

    // Create FreeRTOS Tasks
    xTaskCreatePinnedToCore(
        TaskReadSensors,
        "ReadSensorsTask",
        4096,
        NULL,
        1,
        NULL,
        0
    );

    xTaskCreatePinnedToCore(
        TaskMqttPublish,
        "MqttPublishTask",
        4096,
        NULL,
        1,
        NULL,
        1
    );
}

void loop() {
    // FreeRTOS handling all tasks in background
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void TaskReadSensors(void *pvParameters) {
    for (;;) {
        // Read ADC & calculate simulated/real voltage & current
        float vSolar = analogRead(34) * (3.3f / 4095.0f) * 11.0f; // Voltage divider ratio
        float vBat   = analogRead(35) * (3.3f / 4095.0f) * 5.0f;

        if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
            systemData.solarVoltage = vSolar;
            systemData.batteryVoltage = vBat;
            
            // FSM Logic
            if (systemData.batteryVoltage < 12.6f) {
                systemData.state = BULK;
            } else if (systemData.batteryVoltage >= 12.6f && systemData.batteryVoltage < 14.4f) {
                systemData.state = ABSORPTION;
            } else {
                systemData.state = FLOAT;
            }
            
            xSemaphoreGive(dataMutex);
        }

        vTaskDelay(pdMS_TO_TICKS(200)); // Sample every 200ms
    }
}

void TaskMqttPublish(void *pvParameters) {
    for (;;) {
        if (!mqttClient.connected()) {
            reconnectMQTT();
        }
        mqttClient.loop();

        JsonDocument doc;
        
        if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
            doc["solar_v"] = systemData.solarVoltage;
            doc["battery_v"] = systemData.batteryVoltage;
            doc["state"] = static_cast<int>(systemData.state);
            xSemaphoreGive(dataMutex);
        }

        char buffer[256];
        serializeJson(doc, buffer);
        mqttClient.publish("solar/telemetry", buffer);

        vTaskDelay(pdMS_TO_TICKS(1000)); // Publish every 1s
    }
}