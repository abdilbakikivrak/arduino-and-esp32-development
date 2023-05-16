#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Servo.h>
#include "ClosedCube_HDC1080.h"

// WiFi network credentials
const char* ssid = "your-ssid";
const char* password = "your-password";

// MQTT broker IP address
const char* mqtt_server = "broker-ip-address";

// MQTT client ID
const char* clientID = "esp32-client";

// MQTT topics
const char* servo_topic = "esp32/servo"; // topic to control servo motor
const char* temp_topic = "esp32/temperature"; // topic to publish temperature
const char* hum_topic = "esp32/humidity"; // topic to publish humidity

// Temperature threshold to activate servo motor (in Celsius)
#define TEMP_THRESHOLD 25 

// Servo motor pin
#define SERVO_PIN 26 

// WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Servo motor and hdc1080 sensor objects
Servo servoMotor;
ClosedCube_HDC1080 hdc1080;

// Function to connect to WiFi network
void setup_wifi() {
  delay(10);
  // Print network name
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to WiFi network
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to reconnect to MQTT broker if disconnected
void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect with client ID
    if (client.connect(clientID)) {
      Serial.println("connected");
      // Subscribe to servo topic
      client.subscribe(servo_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait for 5 seconds before retrying
      delay(5000);
    }
  }
}

// Function to handle incoming messages on subscribed topics
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  // Check if message is on servo topic
  if (strcmp(topic, servo_topic) == 0) {
    // Convert payload to string
    String message;
    for (int i = 0; i < length; i++) {
      message += (char)payload[i];
    }
    // Print message
    Serial.println(message);

    // Check if message is "on" or "off"
    if (message == "on") {
      // Rotate servo motor to 90 degrees
      servoMotor.write(90);
      Serial.println("Servo motor activated");
    }
    else if (message == "off") {
      // Rotate servo motor to 0 degrees
      servoMotor.write(0);
      Serial.println("Servo motor deactivated");
    }
  }
}

void setup() {
  Serial.begin(9600);

  // Initialize hdc1080 sensor
  hdc1080.begin(0x40);

  // Initialize servo motor
  servoMotor.attach(SERVO_PIN);

  // Connect to WiFi network
  setup_wifi();

  // Set MQTT server and callback function
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  // Reconnect to MQTT broker if disconnected
  if (!client.connected()) {
    reconnect();
  }
  // Handle incoming messages
  client.loop();

  // Read temperature and humidity from sensor
  float temperature = hdc1080.readTemperature();
  float humidity = hdc1080.readHumidity();

  // Print values to serial monitor
  Serial.print("T=");
  Serial.print(temperature);
  Serial.print("C, RH=");
  Serial.print(humidity);
  Serial.println("%");

  // Publish values to MQTT topics
  client.publish(temp_topic, String(temperature).c_str());
  client.publish(hum_topic, String(humidity).c_str());

  // Check if temperature is above threshold
  if (temperature > TEMP_THRESHOLD) {
    // Publish "on" message to servo topic
    client.publish(servo_topic, "on");
    Serial.println("Servo motor activated");
  }
  else {
    // Publish "off" message to servo topic
    client.publish(servo_topic, "off");
    Serial.println("Servo motor deactivated");
  }

  // Wait for 3 seconds
  delay(3000);
}