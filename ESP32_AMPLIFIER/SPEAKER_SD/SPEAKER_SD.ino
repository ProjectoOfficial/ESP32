/*
 * Datasheet: https://np.micro-semiconductor.hk/datasheet/af-HW04505200J0G.pdf
 * Copyright (C) Dott. Daniel Rossi
 *
*/

#include "Audio.h" //https://github.com/schreibfaul1/ESP32-audioI2S
#include <SPI.h>
#include "SD.h"
#include "FS.h" // File system wrapper

#define SD_CS          5
#define SPI_MOSI      23
#define SPI_MISO      19
#define SPI_SCK       18
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26


Audio audio;

void setup() {
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  Serial.begin(115200);

  if (!SD.begin(SD_CS)) {
    Serial.println("initialization failed. You should ckeck:");
    Serial.println("  - SD card wiring (MISO, MOSI, SCLK) on your board's datasheet");
    Serial.println("  - CS pin wiring and Pin declared in code");
    Serial.println("  - if SD card has been inserted");
    while (1);
  }

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(15); // 0...21
  audio.connecttoFS(SD, "musica1.mp3");
}

void printDirectory(File dir) {
  while (true) {
    File entry =  dir.openNextFile();
    if (!entry) {
      break;
    }

    Serial.println(entry.name());

    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry);
    }
    entry.close();
  }
}

void loop() {
  audio.loop();
  if (Serial.available()) {
    audio.stopSong();

    Serial.println("");
    String r = Serial.readString();
    r.trim();
    char *buff = (char *) calloc(r.length() + 1, sizeof(char));
    r.toCharArray(buff, r.length() + 1);

    File root = SD.open("/");
    printDirectory(root);

    Serial.print("PLAYING: ");
    Serial.print(r);
    Serial.println("");
    Serial.println("");
    audio.connecttoFS(SD, buff);
    log_i("free heap=%i", ESP.getFreeHeap());
  }
}

// PRINT ON SERIAL MONITOR ABOUT MUSIC TRACK
void audio_info(const char *info) {
  Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info) { //id3 metadata
  Serial.print("id3data     "); Serial.println(info);
}
void audio_eof_mp3(const char *info) { //end of file
  Serial.print("eof_mp3     "); Serial.println(info);
}
void audio_showstation(const char *info) {
  Serial.print("station     "); Serial.println(info);
}
void audio_showstreaminfo(const char *info) {
  Serial.print("streaminfo  "); Serial.println(info);
}
void audio_showstreamtitle(const char *info) {
  Serial.print("streamtitle "); Serial.println(info);
}
void audio_bitrate(const char *info) {
  Serial.print("bitrate     "); Serial.println(info);
}
void audio_commercial(const char *info) { //duration in sec
  Serial.print("commercial  "); Serial.println(info);
}
void audio_icyurl(const char *info) { //homepage
  Serial.print("icyurl      "); Serial.println(info);
}
void audio_lasthost(const char *info) { //stream URL played
  Serial.print("lasthost    "); Serial.println(info);
}
void audio_eof_speech(const char *info) {
  Serial.print("eof_speech  "); Serial.println(info);
}
