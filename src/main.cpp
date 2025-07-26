#include <Arduino.h>
#include <nfc.h>
#include <led.h>
#include <WiFi.h>
#include <webserver.h>

#define I2C_SDA 6
#define I2C_SCL 7
#define PN532_IRQ -1
#define PN532_RESET -1



void setup()
{
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_AP);
    String ssid = "Klausurampel_" + WiFi.macAddress();
    WiFi.softAP(ssid.c_str());

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    nfc_setup();
    led_setup();
    webserver_setup();
}

void loop()
{
    nfc_loop();
}
