

#include <WiFi.h>
#include <PubSubClient.h>

#include <TinyGPS++.h>
#define GPS_TX 16
#define GPS_RX 17
//ESP32 MQTT kullanıcı yapılandırması
const char* ssid = "kivrak_system"; // Wifi SSID
const char* password = "adminkivrak"; // Wifi şifre
const char* username = "adminkivrak1"; //  username
const char* pubTopic = "publish/adminkivrak1/SpfL86SJ3NzgYXCqrEUSYor1b49EBZ84"; // publish/username/apiKeyIn
const unsigned int writeInterval = 25000; //yazma aralığı (in ms)
static const uint32_t GPSBaud = 9600;
//static const int RXPin = 16, TXPin = 17;

//AskSensors MQTT yapılandırma
const char* mqtt_server = "mqtt.asksensors.com";
unsigned int mqtt_port = 1883;

// wifi ve mqtt için client tanımları
WiFiClient askClient;
PubSubClient client(askClient);
TinyGPSPlus gps; 
//SoftwareSerial ss(RXPin, TXPin); // serial baglantı 

// setup
void setup() {
Serial.begin(115200);
Serial.println("*****************************************************");
Serial.println("**********Program Başlangici : ESP32, NEO-6M GPS konumunu MQTT üzerinden AskSensors'a yayinlar");
Serial.print("**********  WIFI agina baglaniyor : ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("->WiFi Baglandii");
Serial.println("->IP adresi: ");
Serial.println(WiFi.localIP());

client.setServer(mqtt_server, mqtt_port);
client.setCallback(callback);
// GPS baud hızı
Serial2.begin(GPSBaud);

}
// loop dongusu
void loop() {

if (!client.connected()) // Eğer istemci bağlı değilse
reconnect();
client.loop(); //MQTT istemcisini güncelle

// 
while (Serial2.available() > 0) //seri port 2'den gelen verileri okumak için bir while döngüsü başlatılır eğer veri varsa 
if (gps.encode(Serial2.read())) // Okunan veriyi GPS nesnesi eğer geçerli bir GPS cümlesi ise seri
displayInfo();

if (millis() > 5000 && gps.charsProcessed() < 10)
{
Serial.println(F("GPS algilanmadi: kablolamayi kontrol edin."));
while(true);
}
}

// GPS baglantı kontrol
void displayInfo() {

if (gps.location.isValid()) {
double latitude = (gps.location.lat());
double longitude = (gps.location.lng());

Serial.println("********** MQTT verilerini ASKSENSORS'ta yayinlayin");
char mqtt_payload[50] = "";
snprintf (mqtt_payload, 50, "module1=%lf;%lf", latitude, longitude);
Serial.print("Mesaji yayinla: ");
Serial.println(mqtt_payload);
client.publish(pubTopic, mqtt_payload);
Serial.println("> MQTT verileri yayınlandı");
Serial.println("********** Bitti ");
Serial.println("*****************************************************");

delay(writeInterval);// delay 
} else {
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
//MQTT yeniden baglanma
void reconnect() {

while (!client.connected()) {   // mqtt bağlı değilse
Serial.print("********** Attempting MQTT connection...");
// bağlanmayı dene
if (client.connect("ESP32Client", username, "")) { 
Serial.println("-> MQTT client connected");
} else {
Serial.print("failed, rc=");
Serial.print(client.state());
Serial.println("-> try again in 5 seconds");
// 5 saniye arayla dene 
delay(5000);
}
}
}