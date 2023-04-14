// Kütüphaneleri Dahil Et
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

// Tanımlamalar yapılıyor
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht( DHTPIN, DHTTYPE);
long sonMsg = 0;

// WiFi Tanımlamaları
const char* wifi_id = "**********";// WİFİ SSİD
const char* sifre = "**********";// SSİD PSSWRD
const char* mqtt_sunucu = "127.0.0.1:1880";

// WiFi ve PubSubClient Ataması
WiFiClient espIstemci;
PubSubClient client(espIstemci);

void setup() {
  // Serial portunu başlat
  Serial.begin(115200);

  // DHT Sensörü Başlat
  dht.begin();

  // "setup_wifi" isimli VOID'i Çağır
  setup_wifi();

  // PubSubClient için MQTT Sunucusu ve Portunu Ayarla
  client.setServer(mqtt_sunucu, 1883);
}

void setup_wifi() {
  delay(10);
  // WiFi Ağına Bağlan
  Serial.print(wifi_id);
  Serial.print(" isimli WiFi ağına bağlanıyor");

  WiFi.begin(wifi_id, sifre);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Bağlandı!");
  Serial.print("IP adresi: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void reconnect() {
  // MQTT sunucusuna bağlan
  while (!client.connected()) {
    Serial.print("MQTT sunucusuna bağlanılıyor...");
    // Bağlantı deneniyor
    if (client.connect("ESP8266Client")) {
      Serial.println("Bağlandı!");
      Serial.println();
      // Abone ol ve bilgi gönder
      client.subscribe("esp32/cikis");
      client.publish("esp32/cikis", "MQTT bağlantısı yapıldı!");
    } else {
      Serial.print("bağlanamadı, rc=");
      Serial.print(client.state());
      Serial.println(" | 5 saniye içinde tekrar denenecek");
      // Tekrar denemek için 5 saniye bekle
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - sonMsg > 5000) {
    sonMsg = now;

  float isi = dht.readTemperature();
  float nem = dht.readHumidity();

  if (isnan(nem) || isnan(isi)) {
    Serial.println(F("DHT sensöründen bilgi okunamadı!"));
    return;
  }

  char isiString[8];
    dtostrf(isi, 1, 2, isiString);
    Serial.print("Sıcaklık: ");
    Serial.print(isiString);
    Serial.print("°C - ");
    client.publish("esp32/isi", isiString);
  
  char nemString[8];
    dtostrf(nem, 1, 2, nemString);
    Serial.print("Nem: ");
    Serial.print(nemString);
    Serial.println("%");
    client.publish("esp32/nem", nemString);
  }
}
