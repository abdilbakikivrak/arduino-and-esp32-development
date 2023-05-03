


#include <WiFi.h>
#include <PubSubClient.h>
#include <HardwareSerial.h>

#include <TinyGPS++.h>

//TODO: ESP32 MQTT user config
const char* ssid = "************"; // Wifi SSID
const char* password = "********"; // Wifi Password
const char* username = "*******"; // my AskSensors username
const char* pubTopic = "publish/**********/EZ2XWSVd2e7hOF2ynOTgR5uwGl2aMO1Y/Module1"; // publish/username/apiKeyIn<<<<-------
const unsigned int writeInterval = 25000; // write interval (in ms)

static const int RXPin = 16, TXPin = 17;
static const uint32_t GPSBaud = 9600;
//AskSensors MQTT config
const char* mqtt_server = "mqtt.asksensors.com";
unsigned int mqtt_port = 1883;

// objects
WiFiClient askClient;
PubSubClient client(askClient);
TinyGPSPlus tinygps;
//TinyGPSPlus gps; // The TinyGPS++ object
//HardwareSerial ss(2); // The serial connection to the GPS device
//ss.begin(115200);
//HardwareSerial gps(1);
 //HardwareSerial Serial1(1);
// setup
void setup() {
Serial.begin(9600);
Serial.println("*****************************************************");
Serial.println("********** Program Start : ESP32 publishes NEO-6M GPS position to AskSensors over MQTT");
Serial.print("********** connecting to WIFI : ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("->WiFi connected");
Serial.println("->IP address: ");
Serial.println(WiFi.localIP());

client.setServer(mqtt_server, mqtt_port);
client.setCallback(callback);
// GPS baud rate
Serial1.begin(GPSBaud); 
//ss.begin(GPSBaud);

}
// loop
void loop() {

if (!client.connected())
reconnect();
client.loop();
// This sketch displays information every time a new sentence is correctly encoded.
//while (ss.available() > 0)
while (Serial1.available() > 0)
//if (gps.encode(ss.read()))
if (tinygps.encode(Serial1.read())) 
displayInfo();

//if (millis() > 5000 && gps.charsProcessed() < 10)
if (millis() > 5000 && tinygps.charsProcessed() < 10)
{
Serial.println(F("No GPS detected: check wiring."));
while(true);
}
}
//gps
// GPS displayInfo
void displayInfo() {

if (tinygps.location.isValid()) {
double latitude = (tinygps.location.lat());
double longitude = (tinygps.location.lng());

Serial.println("********** Publish MQTT data to ASKSENSORS");
char mqtt_payload[50] = "";
snprintf (mqtt_payload, 50, "m1=%lf;%lf", latitude, longitude);
Serial.print("Publish message: ");
Serial.println(mqtt_payload);
client.publish(pubTopic, mqtt_payload);
Serial.println("> MQTT data published");
Serial.println("********** End "); 
      Serial.println("*****************************************************");

delay(writeInterval);// delay
} 
else 
{
Serial.println(F("INVALID"));
}
//ss
}

//MQTT callback
void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}
Serial.println();
}
//MQTT reconnect
void reconnect() {
// Loop until we're reconnected
while (!client.connected()) {
Serial.print("********** Attempting MQTT connection...");
// Attempt to connect
if (client.connect("ESP32Client", username, "")) {
Serial.println("-> MQTT client connected");
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println("-> try again in 5 seconds");
// Wait 5 seconds before retrying
delay(5000);
}
}
}
