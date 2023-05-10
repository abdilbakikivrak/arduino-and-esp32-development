// Kütüphaneleri dahil edin
#include <Adafruit_HDC1000.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi ağınızın SSID ve şifresini girin
const char* ssid = "your-ssid";
const char* password = "your-password";

// MQTT broker'ınızın IP adresini ve port numarasını girin
const char* mqtt_server = "your-mqtt-broker-ip";
const int mqtt_port = 1883;

// ESP32'nin yayınlayacağı MQTT konularını tanımlayın
const char* temperature_topic = "esp32/hdc1080/temperature";
const char* humidity_topic = "esp32/hdc1080/humidity";

// WiFi ve MQTT istemcilerini oluşturun
WiFiClient espClient;
PubSubClient client(espClient);

// HDC1080 sensörünü oluşturun
Adafruit_HDC1000 hdc = Adafruit_HDC1000();

// WiFi bağlantısını kurmak için fonksiyon
void setup_wifi() {
  delay(10);
  // Seri monitöre bağlantı durumunu yazın
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // WiFi ağına bağlanın
  WiFi.begin(ssid, password);

  // Bağlantı kurulana kadar bekleyin
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Seri monitöre bağlantı bilgilerini yazın
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// MQTT broker'ına bağlanmak için fonksiyon
void reconnect() {
  // Bağlantı kurulana kadar deneyin
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Rastgele bir istemci kimliği oluşturun
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Broker'a bağlanmaya çalışın
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Burada abone olmak istediğiniz konuları ekleyebilirsiniz
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // 5 saniye sonra tekrar deneyin
      delay(5000);
    }
  }
}

void setup() {
  // Seri iletişimi başlatın
  Serial.begin(115200);
  
  // Sensörü başlatın ve seri monitöre yazın
  if (!hdc.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1);
  }
  
  // WiFi bağlantısını kurun
  setup_wifi();
  
  // MQTT broker'ının IP adresini ve port numarasını ayarlayın
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {

  // MQTT broker'ına bağlı olup olmadığını kontrol edin
  if (!client.connected()) {
    reconnect();
  }
  
  // MQTT istemcisini çalıştırın
  client.loop();

  // Sensörden sıcaklık ve nem verilerini okuyun
  float temperature = hdc.readTemperature();
  float humidity = hdc.readHumidity();

  // Verileri seri monitöre yazın
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" *C");
  
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  
  // Verileri MQTT konularına yayınlayın
  client.publish(temperature_topic, String(temperature).c_str());
  client.publish(humidity_topic, String(humidity).c_str());
  delay(10000);
}