 #include <Arduino.h>
#include <nfc.h>
#include <led.h>
#include <WiFi.h>
#include <webserver.h>

#define I2C_SDA 6
#define I2C_SCL 7
#define PN532_IRQ -1
#define PN532_RESET -1

volatile bool ledBusy = false; // globaler Status, ob LED gerade blinkt

void startLedBlink() {
  ledBusy = true;
  // Hier deinen LED-Blink-Code starten (Timer, Task, whatever)
}

void stopLedBlink() {
  ledBusy = false;
  // Hier alles beenden, wenn LED fertig geblinkt hat
}



void setup()
{
    Serial.begin(115200);
    delay(1000);
    pinMode(3,OUTPUT);
    digitalWrite(3,HIGH);

    WiFi.mode(WIFI_AP);
    String ssid = "Klausurampel_" + WiFi.macAddress();
    WiFi.softAP(ssid.c_str());

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Wichtig: SPIFFS zuerst mounten
    webserver_setup(); // Hier wird SPIFFS gemountet und Timer-Config erstellt
    
    // Dann NFC setup (lädt Timer-Config)
    nfc_setup();
    
    // LED setup zuletzt
    led_setup();
}

void loop()
{
    nfc_loop();
    
}

