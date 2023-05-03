#include <WiFi.h>
#include <PubSubClient.h>

//TODO: ESP32 MQTT user config
const char* ssid = "kivrak_system"; // Wifi ID
const char* password = "adminkivrak"; // Wifi şifre
const char* username = "adminkivrak1"; //  username
const char* subTopic = "actuator/adminkivrak1/Fj5dv7xKXOtMLr2i7gEkpMW8n43yjiZw"; // actuator/username/apiKeyOut
//const int LED_pin = 2; // LEd pin
#define LED_pin 22

const char* mqtt_server = "mqtt.asksensors.com";
unsigned int mqtt_port = 1883;

WiFiClient askClient;
PubSubClient client(askClient);

void setup() {
Serial.begin(115200);
Serial.println("*****************************************************");
Serial.println("********** Program Başlatma : ESP32, MQTT üzerinden AskSensors ile LED'i kontrol et");
Serial.println("LED'i çıkış olarak ayarla");
pinMode(LED_pin, OUTPUT); // led çıkısı ayarla 

Serial.print("**********  WIFI ye baglaniyor: ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("->WiFi Baglandii ");
Serial.println("->IP adresi: ");
Serial.println(WiFi.localIP());

client.setServer(mqtt_server, mqtt_port);
client.setCallback(callback);

if (!client.connected()) 
reconnect();
Serial.print("********** AskSensors aktüatör konusuna abone olun:");
Serial.print(subTopic);
// üyelik
client.subscribe(subTopic);
}

void loop() {
client.loop();
}


void callback(char* topic, byte* payload, unsigned int length) {

Serial.print("AskSensors'tan alınan komut[");
Serial.print(topic);
Serial.print("] ");
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}

Serial.println("********** gelen komutu ayristir ");

String command = String((char*)payload); 

if(command == "module1=1"){ 
  
digitalWrite(LED_pin, HIGH); 
Serial.println("LED Acik");

} else  {

digitalWrite(LED_pin, LOW); // 
Serial.println("LED Kapali");
}
} /*
  String command = String((char*)payload); // add this line
  
  digitalWrite(LED_pin, !command.toInt());
  
  if(command.toInt() == HIGH){
    Serial.println("LED is ON");
  }
  else{
    Serial.println("LED is OFF");
  }
} */

void reconnect() {
//  yenidenbaglan
while (!client.connected()) {
Serial.print("********** MQTT bağlantisi deneniyor...");

if (client.connect("ESP32Client", username, "")) { 
Serial.println("-> MQTT istemcisi baglandi");
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println("-> bes saniyede tekrar dene");
// 5 sn bekle 
delay(5000);
}
}
}