#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "**************";// SSID WİFİ İSMİ
const char* password = "*******"; //SSİD WİFİ ŞİFRESİ
const char* mqtt_server = "***********:1883";// mqtt server ip adresi
const int mqtt_port = 1883; // MQTT PORT
const char* mqtt_user = "******"; // mqtt kullanıcı adı
const char* mqtt_password = "******"; // mqtt parolası

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // WiFi ağına bağlanma
  Serial.println();
  Serial.print("Bağlanılıyor: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi bağlandı");
  Serial.println("IP adresi: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Konu: ");
  Serial.println(topic);
  Serial.print("Yük: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Hesap bilgilerini kullanarak MQTT broker'ına bağlanma
  while (!client.connected()) {
    Serial.print("MQTT broker'ına bağlanılıyor...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("bağlandı");
      client.subscribe("esp32/data");
    } else {
      Serial.print("bağlanılamadı, hata kodu= ");
      Serial.print(client.state());
      Serial.println(" 5 saniye sonra yeniden denenecek..");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
