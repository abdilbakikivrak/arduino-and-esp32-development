// Include required libraries
#include <WiFi.h>
#include <PubSubClient.h>
#include <TinyGPS++.h>

// WiFi credentials
const char* ssid = "kivrak_system";
const char* password = "adminkivrak";

// MQTT server credentials
const char* mqtt_server = "mqtt.asksensors.com";// const char* mqtt_server = "mqtt.asksensors.com";
const int mqtt_port = 1883;
const char* mqtt_user = "adminkivrak1";
const char* mqtt_password = "53bkEaSSa11";

// AskSensors API KEY IN
const char* apiKeyIn = "SpfL86SJ3NzgYXCqrEUSYor1b49EBZ84";

// GPS module TX and RX pins
#define GPS_TX 16
#define GPS_RX 17

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a WiFi client object
WiFiClient wifiClient;

// Create a PubSubClient object
PubSubClient client(wifiClient);

// Connect to WiFi network
void setup_wifi() {
  delay(10);
  // Print to serial monitor
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Begin WiFi connection
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print connection details
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect to MQTT server
void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Subscribe to topic
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Setup function
void setup() {
  // Initialize serial communication at baud rate of 115200
  Serial.begin(115200);
  
  // Initialize serial communication with GPS module at baud rate of 9600
  Serial2.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);

  // Setup WiFi connection
  setup_wifi();

  // Set MQTT server and port
  client.setServer(mqtt_server, mqtt_port);
}

// Loop function
void loop() {
  
  // Reconnect to MQTT server if needed
  if (!client.connected()) {
    reconnect();
  }
  
  // Process MQTT messages
  client.loop();

  // Read data from GPS module
  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
    if (gps.location.isUpdated()) {
      // Print latitude and longitude to serial monitor
      Serial.print("Latitude= ");
      Serial.print(gps.location.lat(), 6);
      Serial.print(" Longitude= ");
      Serial.println(gps.location.lng(), 6);

      // Publish latitude and longitude to AskSensors over MQTT
      String payload = String(apiKeyIn) + "&module1=" + String(gps.location.lat(),6) + "&module2=" + String(gps.location.lng(),6);
      client.publish("module1", payload.c_str());
      Serial.print("Veri gönderildi: ");
      Serial.println(payload);
    }
    
    // Wait for a second before next iteration
    delay(1000);
    
   }
}
