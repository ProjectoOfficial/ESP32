#include <WiFi.h>
#include <FirebaseESP32.h>

#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""

#define WIFI_SSID ""
#define WIFI_PASSWORD ""

FirebaseData fData;
FirebaseJson json;

size_t updateTime = 0;

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  size_t start_time = millis();
  while (WiFi.status() != WL_CONNECTED)
    if (millis() - start_time > 500) {
      Serial.print(".");
      start_time = millis();
    }

  Serial.println("Connessione");
  Serial.println();
  Serial.print("WiFi Connesso, indirizzo IP:");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(fData, 1000 * 60);
  Firebase.setwriteSizeLimit(fData, "tiny");

  Serial.println();
  Serial.println("-------------------------------------");
  Serial.println("Fatto! WiFi connesso!");
  updateTime = millis();
}

void loop() {
  if (millis() - updateTime > 500) {
    int val1 = random(300);
    int val2 = random(300);
    Serial.print("Valore 1:");
    Serial.print(val1);
    Serial.print(" - Valore 2:");
    Serial.println(val2);
    json.set("/data1", val1);
    json.set("/data2", val2);
    Firebase.updateNode(fData, "/valore", json);
    updateTime = millis();
  }
}
