#include <TinyGPS++.h>
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "index.h"

#define SWITCH 23
#define MODE_MAX 7
#define READ_DELAY 200
#define UPDATE_DELAY 1
#define DISPLAY_DELAY 50

TinyGPSPlus gps;

LiquidCrystal lcd(13, 12, 14, 27, 26, 25);

short MODE = 0;
unsigned long time_delay;
unsigned long disp_time;

double MOVE_X = 0.0;
double MOVE_Y = 0.0;

String X = "44.6283916";
String Y = "10.9497243";
String Z = "20z";

String ALT = "0.0m";

// WEB SERVER
#define WIFI_MAX_WAIT 5000

const char* ssid = "";
const char* password = "";

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void runserver() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", home0_html);
  });

  server.on("/getPos", HTTP_GET, [](AsyncWebServerRequest * request) {
    String pos = String("https://www.google.com/maps/d/u/0/embed?mid=1OWjIiXc7clGxGdbrHvjNOdY9ZLo&ll=" + X + "%2C" + Y + "&z=" + Z);
    request->send_P(200, "text/plane", pos.c_str());
  });

  server.on("/getLat", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plane", X.c_str());
  });

  server.on("/getLon", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plane", Y.c_str());
  });

  server.on("/getAlt", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plane", ALT.c_str());
  });
}

/*GLOBAL VARIABLES */
unsigned long satellites = 0;
double latitude = 0.0;
double longitude = 0.0;
double altitude = 0.0;
double speed = 0.0;
double course = 0.0;
double hdop = 0.0;

unsigned long satellites_age = 0;
unsigned long location_age = 0;
unsigned long course_age = 0;
unsigned long altitude_age = 0;
unsigned long hdop_age = 0;

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17, false);
  lcd.begin(16, 2);

  pinMode(SWITCH, INPUT);

  lcd.noAutoscroll();
  lcd.noBlink();
  lcd.noCursor();

  lcd.clear();
  delay(UPDATE_DELAY);
  lcd.home();
  lcd.print("Land Meter");
  lcd.setCursor(0, 1);
  lcd.print("By Dani");
  delay(1500);

  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  lcd.clear();
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print(ssid);

  WiFi.begin(ssid, password);
  unsigned long connect_time = millis();
  while ((WiFi.status() != WL_CONNECTED) && (millis() - connect_time < WIFI_MAX_WAIT)) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  if (WiFi.status() == WL_CONNECTED) {
    lcd.print("Connected:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(1500);
    runserver();
    server.onNotFound(notFound);
    server.begin();
  }
  else {
    lcd.print("no connection");
    delay(1500);
  }

  lcd.clear();
  delay(UPDATE_DELAY);

  time_delay = millis();
  disp_time = millis();
  Serial.println("Started!");
}

void lcd_print(const String str1, const String str2) {
  lcd.home();
  lcd.print(str1);
  delay(UPDATE_DELAY);
  lcd.setCursor(0, 1);
  lcd.print(str2);
  delay(UPDATE_DELAY);
}

void loop() {
  while (Serial2.available()) {
    Serial.println("Data");
    gps.encode(Serial2.read());
  }

  if (millis() - disp_time > DISPLAY_DELAY) {

    // UPDATE GPS VALS
    if (gps.satellites.isUpdated()) { // SATELLITES
      satellites_age = gps.satellites.age();
      satellites = gps.satellites.value();
    }

    if (gps.location.isUpdated()) { // LOCATION
      latitude = gps.location.lat();
      longitude = gps.location.lng();
      location_age = gps.location.age();

      X = String(latitude + MOVE_X, 8);
      Y = String(longitude + MOVE_Y, 8);
    }

    if (gps.altitude.isUpdated()) { // ALTITUDE
      altitude_age = gps.altitude.age();
      altitude = gps.altitude.meters();
      ALT = String(altitude, 2) + "m";
    }

    if (gps.speed.isUpdated()) // SPEED
      speed = gps.speed.mps();


    if (gps.course.isUpdated()) { // COURSE
      course = gps.course.deg();
      course_age = gps.course.age();
    }

    if (gps.hdop.isUpdated()) { // HDOP
      hdop_age = gps.hdop.age();
      hdop = gps.hdop.hdop();
    }

    // BUTTON CONTROL
    if (MODE == 0)
      lcd_print((String)"SATs [" + satellites_age + "ms]      ", (String)satellites);

    else if (MODE == 1) {
      lcd.home();
      lcd.print(String(latitude + MOVE_X, 7));
      delay(UPDATE_DELAY);
      lcd.setCursor(0, 1);
      lcd.print(String(longitude + MOVE_Y, 7));
      delay(UPDATE_DELAY);
    }

    else if (MODE == 2) {
      lcd.home();
      lcd.print((String)"LAT [" + location_age + "ms]       ");
      delay(UPDATE_DELAY);
      lcd.setCursor(0, 1);
      lcd.print(String(latitude + MOVE_X, 7));
      delay(UPDATE_DELAY);
    }

    else if (MODE == 3) {
      lcd.home();
      lcd.print((String)"LON [" + location_age + "ms]       ");
      delay(UPDATE_DELAY);
      lcd.setCursor(0, 1);
      lcd.print(String(longitude + MOVE_Y, 7));
      delay(UPDATE_DELAY);
    }

    else if (MODE == 4) {
      lcd.home();
      lcd.print((String)"SPEED [" + location_age + "ms]      ");
      delay(UPDATE_DELAY);
      lcd.setCursor(0, 1);
      lcd.print(String(speed, 2) + "m/s");
      delay(UPDATE_DELAY);
    }

    else if (MODE == 5) {
      lcd_print((String)"COURSE [" + course_age + "ms]     ", String(course, 4));
    }

    else if (MODE == 6) {
      lcd_print((String)"ALT [" + altitude_age + "ms]     ", String(altitude, 2) + "m");
    }

    else if (MODE == 7) {
      lcd_print((String)"HDOP [" + hdop_age + "ms]     ",  String(hdop, 4));
    }

    else if (gps.charsProcessed() < 10)
      Serial.println("WARNING: No GPS data.  Check wiring.");
    disp_time = millis();
  }

  if (millis() - time_delay > READ_DELAY) {
    if (digitalRead(SWITCH)) {
      if (MODE < MODE_MAX)
        ++MODE;
      else
        MODE = 0;
      lcd.clear();
      lcd.noCursor();

      time_delay = millis();
      delay(UPDATE_DELAY);
    }
  }
}
