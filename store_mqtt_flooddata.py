import paho.mqtt.client as mqtt
import csv
from datetime import datetime

# MQTT configuration
mqtt_broker = "localhost"             # Mosquitto broker on the VM
mqtt_topic = "flood-data"             # Topic to subscribe to

# CSV file configuration
csv_filename = 'sensor_data.csv'

# Initialize the CSV file with headers if it doesn't exist
try:
    with open(csv_filename, mode='x', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['timestamp', 'data'])  # Write header row
except FileExistsError:
    pass  # If file already exists, do nothing

# Callback for when the client connects to the MQTT broker
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
        client.subscribe(mqtt_topic)
    else:
        print(f"Failed to connect, return code {rc}")

# Callback for when a message is received
def on_message(client, userdata, message):
    try:
        payload = message.payload.decode()  # Decode the message
        print(f"Received message: {payload}")

        # Write the data into CSV with a timestamp
        with open(csv_filename, mode='a', newline='') as file:
            writer = csv.writer(file)
            writer.writerow([datetime.utcnow(), payload])  # Write timestamp and data
        print("Data appended to CSV file.")

    except Exception as e:
        print(f"Error processing message: {e}")

# Initialize MQTT client
mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message

# Connect to the MQTT broker
mqtt_client.connect(mqtt_broker, 1883, 60)

# Start the MQTT client loop
mqtt_client.loop_forever()