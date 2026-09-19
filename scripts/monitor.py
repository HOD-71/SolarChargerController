import json
import logging
import paho.mqtt.client as mqtt

# Configure professional logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

MQTT_BROKER = "localhost"  # Docker Mosquitto Container IP/Hostname
MQTT_PORT = 1883
TOPIC = "solar/telemetry"

# FSM State Mapper (Sync with C++ enum)
STATE_MAP = {
    0: "BULK",
    1: "ABSORPTION",
    2: "FLOAT",
    3: "FAULT"
}

def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        logging.info("Successfully connected to Mosquitto MQTT Broker.")
        client.subscribe(TOPIC)
        logging.info(f"Subscribed to topic: '{TOPIC}'")
    else:
        logging.error(f"Failed to connect to MQTT Broker, return code: {rc}")

def on_message(client, userdata, msg):
    try:
        # Parse JSON Telemetry
        payload = json.loads(msg.payload.decode('utf-8'))
        
        solar_v = payload.get("solar_v", 0.0)
        battery_v = payload.get("battery_v", 0.0)
        state_code = payload.get("state", 3)
        
        state_str = STATE_MAP.get(state_code, "UNKNOWN")
        
        # Log formatted telemetry data
        logging.info(
            f"PV Voltage: {solar_v:5.2f}V | "
            f"Battery Voltage: {battery_v:5.2f}V | "
            f"Charger State: {state_str}"
        )
        
    except json.JSONDecodeError:
        logging.error("Failed to decode JSON payload received from broker.")
    except Exception as e:
        logging.error(f"Unexpected error processing message: {e}")

def main():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message

    logging.info("Starting Solar Charger Telemetry Consumer...")
    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        client.loop_forever()
    except KeyboardInterrupt:
        logging.info("Disconnecting from broker and shutting down...")
        client.disconnect()
    except Exception as e:
        logging.error(f"Connection error: {e}")

if __name__ == "__main__":
    main()