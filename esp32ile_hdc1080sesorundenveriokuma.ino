// ESP32 ile HDC 1080 sensöründen veri okuyan ve seri monitöre yazan kod

// Kütüphaneleri dahil et
#include <Wire.h>
#include <Adafruit_HDC1000.h>

// Sensör nesnesini tanımla
//ClosedCube_HDC1080 hdc1080;
Adafruit_HDC1000 hdc = Adafruit_HDC1000();
// Kurulum fonksiyonu
void setup() {
  // Seri bağlantıyı başlat
  Serial.begin(115200);
  // I2C bağlantısını başlat
  Wire.begin();
  // Sensörü başlat
  hdc.begin(0x40);
}

// Döngü fonksiyonu
void loop() {
  // Sensörden sıcaklık ve nem verilerini oku
  double temp = hdc.readTemperature();
  double hum = hdc.readHumidity();
  // Verileri seri monitöre yaz
  Serial.print("Sicaklik: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.print("Nem: ");
  Serial.print(hum);
  Serial.println(" %");
  // Bir saniye bekle
  delay(1000);
}
