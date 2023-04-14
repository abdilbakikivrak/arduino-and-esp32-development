#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient
#include <Adafruit_Sensor.h>//https://github.com/adafruit/Adafruit_Sensor
#include <DHT.h>//https://github.com/adafruit/DHT-sensor-library

#define wifi_ssid "FiberHGW_ZTZY2R_2.4GHz"
#define wifi_password "******"

//mqtt sunucu bilgilerimiz.
#define mqtt_server "192.168.1.***"
#define mqtt_user "mqtt_user"
#define mqtt_password "abcdefgh"

//mqtt sunucusunda mesaj yayımlayacağımız konu başlıkları ve altındaki veriler 
#define humidity_topic "sensor/rutubet"
#define temperature_topic "sensor/sicaklik"

WiFiClient espClient;
PubSubClient client(espClient); //bu nesne sayesinde mqtt sunucusu ile bağlantı sağlayacağız.

// DHT sensör D2 pinine bağlı
#define DHTPIN D2    

// DHT tipini seçin
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE); //bir dht nesnesi oluşturuyoruz

// Kodda kullandığımız değişkenler
float temperature;
float humidity;

void setup() {
  Serial.begin(115200);
  //wifi bağlantısını yaptık
  setup_wifi();
  //mqtt sunucusuna bağlanıyoruz.
  client.setServer(mqtt_server, 1883);

}

//Ağ bağlantısı için gerekli fonksiyonumuz
void setup_wifi() {
  delay(10);
  // ağ bağlantısı ayarları
  Serial.println();
  Serial.print("Ağa Bağlanılıyor: ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Ağ Bağlantısı Kuruldu");
  Serial.println("IP adresi: ");
  Serial.println(WiFi.localIP());
}

//mqtt sunucusuna bağlantı için gerekli fonksiyonumuz
void reconnect() {
  // Bağlanana kadar kal
  while (!client.connected()) {
    Serial.print("MQTT sunucusuna bağlanılıyor...");
    // Bağlantı ayarları
    // kullancı adı ve şifre kullanmak istemiyorsanız alttaki satırı bir sonraki satırla değiştirin 
    //if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("mqttye bağlanıldı");
    } else {
      Serial.print("HATA, rc=");
      Serial.print(client.state());
      Serial.println(" 5sn içinde tekrar denenecek");
      // 5sn bekle
      delay(5000);
    }
  }
}

//bu fonksiyonu okunan değerlerde değişiklik yoksa boşuna sunucuyu her seferinde meşgul etmemek için kullanabiliriz.
//önceki değer ile yeni değer arası fark belirlenen max fark değerinden küçükse bir işlem yapmayabiliriz.
bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

//gerekli değişkenlerimizi tanımlıyoruz.
long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;  
float diff = 0.2; //ne kadar değişikliği yeni değer olarak kabul etmek istiyoruz?

//LOOP fonksiyonumuz
void loop() {
  //eğer mqtt sunucusuna bağlı değilse bağlanmak için deneyecek
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

//millis kullanarak her bir saniyede bir olacak şekilde sensörden veri okuyoruz.
  long now = millis();
  if (now - lastMsg > 1000) {
     lastMsg = now;
     temperature = dht.readTemperature();
     humidity = dht.readHumidity();

//eğer okunan verilerde diff kadar değişiklik varsa yeni veri olarak alıp yayınlıyoruz.
    if (checkBound(temperature, temp, diff)) {
      temp = temperature;
      Serial.print("Yeni Sıcaklık Değeri:");
      Serial.println(String(temp).c_str());
      client.publish(temperature_topic, String(temp).c_str(), true); //sıcaklık ile ilgili TOPIC'e okunan değeri yayımladık
    }
// aynı şeyler rutubet için de geçerli
    if (checkBound(humidity, hum, diff)) {
      hum = humidity;
      Serial.print("Yeni Rutubet Değeri:");
      Serial.println(String(hum).c_str());
      client.publish(humidity_topic, String(hum).c_str(), true);
    }
  }
}
