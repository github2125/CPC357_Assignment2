#include <Setup.h>
#include "DHT.h"
#include <ESP32Servo.h>

// Sensor and Actuator Pins
const int dht11Pin = 42;     // DHT11 pin
const int moisturePin = A2;  // Moisture sensor pin
const int depthPin = A5;     // Water level sensor pin
const int rainSensorPin = A8; // Rain sensor pin
const int servoPin = A11;    // Servo pin

// LED Pins
const int redledPin = 9;
const int yellowledPin = 5;
const int greenledPin = 4;

// Thresholds
const int rainThreshold = 2600;  // Example rain sensor threshold
const int moistureThreshold = 65; // Moisture threshold
const int depthThreshold = 75;    // Water level threshold

// Variables
char buffer[256]; // Buffer for MQTT payload
DHT dht(dht11Pin, DHT11); // DHT11 sensor
Servo Myservo;

// Wi-Fi and MQTT Clients
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Function to Connect to Wi-Fi
void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to Reconnect MQTT
void reconnect_mqtt() {
  while (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (mqttClient.connect("ESP32Client")) { // Replace with a unique client ID
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// Function to Publish Sensor Data
void publish_sensor_data() {
  // Read sensors
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int rainValue = analogRead(rainSensorPin);
  int moistureValue = analogRead(moisturePin);
  int depthValue = analogRead(depthPin);

  // Map values to percentages if needed
  int mappedMoisture = map(moistureValue, 4095, 1800, 0, 100);
  int mappedDepth = map(depthValue, 0, 1300, 0, 100);

  // Prepare JSON payload
  sprintf(buffer, 
    "{\"humidity\":%.2f,\"temperature\":%.2f,\"rain\":%d,\"moisture\":%d,\"water_level\":%d}", 
    humidity, temperature, rainValue, mappedMoisture, mappedDepth);

  // Publish to MQTT topic
  mqttClient.publish(MQTT_TOPIC, buffer);
  Serial.print("Published: ");
  Serial.println(buffer);

  // Actuator Logic (Example)
  if (mappedDepth > depthThreshold) {
    // Flood state
    digitalWrite(redledPin, HIGH);
    digitalWrite(yellowledPin, LOW);
    digitalWrite(greenledPin, LOW);
    Myservo.write(90); // Open valve
    Serial.println("Flood detected! Valve opened.");
  } else if (mappedMoisture > moistureThreshold) {
    // Warning state
    digitalWrite(redledPin, LOW);
    digitalWrite(yellowledPin, HIGH);
    digitalWrite(greenledPin, LOW);
    Myservo.write(45); // Partial valve opening
    Serial.println("Warning: High moisture levels!");
  } else {
    // Safe state
    digitalWrite(redledPin, LOW);
    digitalWrite(yellowledPin, LOW);
    digitalWrite(greenledPin, HIGH);
    Myservo.write(0); // Close valve
    Serial.println("System in safe state.");
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);

  dht.begin();
  Myservo.attach(servoPin);

  pinMode(redledPin, OUTPUT);
  pinMode(yellowledPin, OUTPUT);
  pinMode(greenledPin, OUTPUT);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect_mqtt();
  }
  mqttClient.loop();

  // Publish sensor data every 5 seconds
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 5000) {
    lastPublish = millis();
    publish_sensor_data();
  }
}
