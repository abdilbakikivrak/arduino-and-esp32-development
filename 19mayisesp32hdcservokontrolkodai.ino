#include <Wire.h>
#include <Adafruit_HDC1000.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "your_wifi_ssid";
const char* password = "your_wifi_password";

// MQTT broker address and credentials
const char* mqtt_server = "your_mqtt_broker_address";
const char* mqtt_user = "your_mqtt_user";
const char* mqtt_password = "your_mqtt_password";

// MQTT topics
const char* temperature_topic = "esp32/temperature";
const char* humidity_topic = "esp32/humidity";
const char* servo_topic = "esp32/servo";

// Servo pin and angle
#define SERVO_PIN 18
#define SERVO_ANGLE 90

// Temperature threshold
#define TEMP_THRESHOLD 25

// HDC1000 sensor object
Adafruit_HDC1000 hdc = Adafruit_HDC1000();

// WiFi client object
WiFiClient espClient;

// MQTT client object
PubSubClient client(espClient);

// Servo object
Servo servo;

// Function to connect to WiFi network
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to reconnect to MQTT broker if connection is lost
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(servo_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Function to publish sensor data to MQTT broker
void publishData() {
  float temperature = hdc.readTemperature();
  float humidity = hdc.readHumidity();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  client.publish(temperature_topic, String(temperature).c_str(), true);
  client.publish(humidity_topic, String(humidity).c_str(), true);

  if (temperature > TEMP_THRESHOLD) {
    servo.write(SERVO_ANGLE);
    Serial.println("Servo turned on");
    client.publish(servo_topic, "on", true);
  } else {
    servo.write(0);
    Serial.println("Servo turned off");
    client.publish(servo_topic, "off", true);
  }
}

// Function to handle incoming messages from MQTT broker
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message;
  
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(message);

  if (String(topic) == servo_topic) {
    if (message == "on") {
      servo.write(SERVO_ANGLE);
      Serial.println("Servo turned on");
    } else if (message == "off") {
      servo.write(0);
      Serial.println("Servo turned off");
    }
    
    client.publish(servo_topic, message.c_str(), true);
    
  }
}

void setup() {
  
  // Initialize serial monitor
  Serial.begin(115200);

   // Initialize HDC1000 sensor
   if (!hdc.begin()) {
     Serial.println("Couldn't find sensor!");
     while (1);
   }

   // Initialize servo motor
   servo.attach(SERVO_PIN);

   // Connect to WiFi network
   setup_wifi();

   // Set MQTT server and callback function
   client.setServer(mqtt_server, 1883);
   client.setCallback(callback);

}

void loop() {

   // Reconnect to MQTT broker if needed
   if (!client.connected()) {
     reconnect();
   }
   
   // Publish sensor data every second
   publishData();
   delay(1000);

   // Handle incoming messages from MQTT broker
   client.loop();
}