/*
  ESP32 WEB SERVER

  This sketch provides an example of creating a Web Server with ESP32 devices. 
  It allow to control a pair of led through two respective buttons on a web page.
  An LM35 make it possible to monitor the temperature, which is showed too on the web page.

  As a style source for HTML, Bootstrap is used.

  The circuit:
    - LED1: GPIO26
    - LED2: GPIO27 
    - LM35: GPIO33

  Created 27/08/2021
  By Dott. Daniel Rossi
  Copyright: ESP32 WEB SERVER © 2021 - GPLv3 
  
  YOUTUBE: Https://youtube.com/c/ProjectoOfficial
  COLLABORATION E-MAIL: miniprojectsofficial@gmail.com
*/

#include <WiFi.h>

#define LED1 26
#define LED2 27
#define LM35 33
#define CONNECT_TIME 10000
#define TIMEOUTTIME 5000

const char* ssid = "Linkem_682B6C";
const char* password = "poletta2598";
WiFiServer server(80);

String header;

String LED1State = "off";
String LED2State = "off";

size_t currentTime = millis();
size_t previousTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LM35, INPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  Serial.print("Attempting to connect to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  size_t start_time = millis();
  size_t dot_time = millis();
  while ((WiFi.status() != WL_CONNECTED) && (start_time + CONNECT_TIME) > millis()) {
    if (dot_time + 250 < millis()) {
      Serial.print(".");
      dot_time = millis();
    }
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    String currentLine = "";
    
    while (client.connected() && currentTime - previousTime <= TIMEOUTTIME) {
      currentTime = millis();
      if (client.available()) {
        String celsius = String(analogRead(LM35) / 9.31);
        char c = client.read();
        Serial.write(c);
        header += c;
        
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /LED1/on") >= 0) {
              Serial.println("LED 1 on");
              LED1State = "on";
              digitalWrite(LED1, HIGH);
            } else if (header.indexOf("GET /LED1/off") >= 0) {
              Serial.println("LED1 off");
              LED1State = "off";
              digitalWrite(LED1, LOW);
            } else if (header.indexOf("GET /LED2/on") >= 0) {
              Serial.println("LED2 on");
              LED2State = "on";
              digitalWrite(LED2, HIGH);
            } else if (header.indexOf("GET /LED2/off") >= 0) {
              Serial.println("LED2 off");
              LED2State = "off";
              digitalWrite(LED2, LOW);
            }
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta charset=\"UTF-8\">");
            client.println("<meta http-equiv=\"refresh\" content=\"5\" >");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\">");
             client.println("<link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.6.3/css/all.css\" integrity=\"sha384-UHRtZLI+pbxtHCWp1t77Bi1L4ZtiqrqD80Kn4Z8NTSRyMA2Fd33n5dQ8lWUE00s/\" crossorigin=\"anonymous\">");
    
            client.println("<script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\"></script>");
            client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\" integrity=\"sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1\" crossorigin=\"anonymous\"></script>");
            client.println("<script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\" integrity=\"sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM\" crossorigin=\"anonymous\"></script>");
            
            client.println("<style>");
            client.println("html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".mybtn {padding: 16px 40px; font-size: 30px;} ");
            client.println(".par { font-size: 16px;}");
            client.println("p {text-align: center;}");
            client.println(".lm35 {text-align: center; border: none; margin: 2px; padding: 16px 40px; font-size: 30px;}");
            client.println("</style></head>");

            client.println("<body class=\"bg-light\">");
            client.println("<h1 style=\"text-align:center; \" class=\"display-4\">ESP32 Web Server </h1> ");
            client.println("<br />");

            client.println("<p class=\"par\">GPIO 26 - State " + LED1State + " </p> ");
            if (LED1State == "off") {
              client.println("<p><a href =\"/LED1/on\" role=\"button\" class=\"btn btn-success mybtn\" >ON</a></p>");
            } else {
              client.println("<p><a href=\"/LED1/off\" role=\"button\" class=\"btn btn-danger mybtn\">OFF</a></p>");
            }
            client.println("<br />");

            client.println("<p class=\"par\">LED2 - State " + LED2State + "</p>");
            if (LED2State == "off") {
              client.println("<p><a href=\"/LED2/on\" role=\"button\" class=\" mybtn btn btn-success mybtn\" >ON</a></p>");
            } else {
              client.println("<p><a href=\"/LED2/off\" role=\"button\" class=\" mybtn btn btn-danger mybtn\">OFF</a></p>");
            }
            client.println("<br />");
            
            double temp = analogRead(LM35) / 9.31;
            String celsius = String(temp );
            client.println("<p class=\"par\">LM35 - Temperature </p>");
            if ( temp < 21)
              client.println("<p> <span class=\"lm35\" style=\"background-color: #198754!important; color:#f8f9fa!important;\"> " + celsius + "°C </span></p>");
            else if ( temp > 24)
              client.println("<p> <span class=\"lm35\" style=\"background-color: #dc3545!important; color:#f8f9fa!important;\"> " + celsius + "°C </span></p>");
            else
              client.println("<p> <span class=\"lm35\" style=\"background-color: #ffc107!important; color:#212529!important;\"> " + celsius + "°C </span></p>");
              
            client.println("<br />");
            client.println("<footer class=\"bg-dark text-center text-white\">");
            client.println("<div class=\"container pt-4 \">");
            client.println("<section class=\"mb-4\">");
            client.println("<a class=\"btn btn-outline-light btn-floating m-1\" href=\"https://www.facebook.com/MiniProjectsOfficial\" target=\"_blank\" rel=\"noopener noreferrer\" role=\"button\">");
            client.println("<i class=\"fab fa-facebook-f\"></i>");
            client.println("</a>");
            client.println("<a class=\"btn btn-outline-light btn-floating m-1\" href=\"https://www.instagram.com/officialprojecto\" target=\"_blank\" rel=\"noopener noreferrer\" role=\"button\">");
            client.println("<i class=\"fab fa-instagram\"></i>");
            client.println("</a>");
            client.println("<a class=\"btn btn-outline-light btn-floating m-1\" href=\"https://youtube.com/c/ProjectoOfficial\" target=\"_blank\" rel=\"noopener noreferrer\" role=\"button\">");
            client.println("<i class=\"fab fa-youtube\"></i>");
            client.println("</a>");
            client.println("<a class=\"btn btn-outline-light btn-floating m-1\" href=\"https://github.com/ProjectoOfficial\" target=\"_blank\" rel=\"noopener noreferrer\" role=\"button\">");
            client.println("<i class=\"fab fa-github\"></i>");
            client.println("</a>");
            client.println("</section>");
            client.println("</div>");
            
            client.println("<div class=\"text-center p-3\" style=\"background-color: rgba(0, 0, 0, 0.2);\"> © 2021 Copyright: <a class=\"text-white\" href=\"https://officialprojecto.wordpress.com\">Projecto</a></div>");
            client.println("</footer>");

            
            client.println("</body></html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
    delay(1);
}
