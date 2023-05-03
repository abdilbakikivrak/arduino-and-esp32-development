#include <WiFi.h>

// TODO: Enter your wifi network credentials
const char* ssid = "kivrak_system";
const char* password = "adminkivrak";

void setup() {
  Serial.begin(115200); // Start serial communication
  WiFi.begin(ssid, password); // Start wifi connection
  Serial.print("Connecting to wifi");
  while (WiFi.status() != WL_CONNECTED) { // Wait until connected
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Wifi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // Print local IP address
}

void loop() {
  // Nothing to do here
}