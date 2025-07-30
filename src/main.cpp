 #include <Arduino.h>
#include <nfc.h>
#include <led.h>
#include <WiFi.h>
#include <webserver.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#define Ringbuffer_SIZE 64 

int RingBuffer[Ringbuffer_SIZE]; 
int buffer_index = 0; 
int sum = 0;


void saveBatteryVoltage(float voltage) {
    File configFile = SPIFFS.open("/config.json", "r");
    StaticJsonDocument<512> doc;
    if (configFile) {
        DeserializationError error = deserializeJson(doc, configFile);
        configFile.close();
        if (error) {
            doc.clear();
        }
    }
    doc["battery_voltage"] = voltage;
    configFile = SPIFFS.open("/config.json", "w");
    if (configFile) {
        serializeJsonPretty(doc, configFile);
        configFile.close();
    }
}


void setup()
{
    Serial.begin(115200);
    delay(1000);
    pinMode(3,OUTPUT);
    digitalWrite(3,HIGH);
    pinMode(2, INPUT);

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

    int input = analogReadMilliVolts(2);
    sum = sum - RingBuffer[buffer_index] + input;
    RingBuffer[buffer_index] = input;
    buffer_index = (buffer_index + 1 ) % Ringbuffer_SIZE;
    Serial.print("Battery voltage: ");
    Serial.println(input);
    
    float battteryVoltage = ((sum / Ringbuffer_SIZE) / 1000.0) * 2; 
   
    saveBatteryVoltage(battteryVoltage); // Wert speichern
}