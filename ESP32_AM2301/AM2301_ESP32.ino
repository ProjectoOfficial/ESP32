#include <DHT.h>

#define DHTPIN 14
#define DHTTYPE AM2301

DHT dht(DHTPIN, DHTTYPE);

float temp = 0.0;
float hum = 0.0;

void setup() {
  Serial.begin(9600);
  dht.begin();

}

void loop() {
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  Serial.print("Temperature: ");Serial.print(temp);Serial.print("°C");
  Serial.print("; Humidity: ");Serial.print(hum);Serial.println("%");
  delay(1000);
}
