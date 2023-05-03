#include <WiFi.h>


const char* ssid = "******************";
const char* password = "***********";

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print(" adrese baglaniyor...");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.wifi agina baglandi');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

}

void setup() {
  Serial.begin(115200);
  initWiFi();
  WiFi.mode(WIFI_STA);
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
  WiFi.disconnect();
    delay(100);

  Serial.println("setup tamamlandi");

}

void loop() {

    Serial.println("tarama basladı");

    // nuraya gore wifi taraması 
    int n = WiFi.scanNetworks();
    Serial.println(" tarama bitti");
    if (n == 0) {
        Serial.println("ag bulunamadı");
    } else {
        Serial.print(n);
        Serial.println(" ag bulundu");
        for (int i = 0; i < n; ++i) {
            
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");


    delay(5000);
}
