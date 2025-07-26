#include <WiFi.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <webserver.h>
#include <nfc.h>
#include <ArduinoJson.h>


static AsyncWebServer server(80);


void webserver_setup() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS konnte nicht gemountet werden");
    return;
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
      request->send(404, "text/html", "<h1>Datei nicht gefunden</h1>");
      return;
    }
    request->send(file, "/index.html", "text/html");
  });

  server.serveStatic("/style.css", SPIFFS, "/style.css");


  // JSON-Rückgabe der aktuellen LED-Farbe
    server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
        RGBColor c = timer.getLastColor();
        String json = "{";
        json += "\"r\":" + String(c.r) + ",";
        json += "\"g\":" + String(c.g) + ",";
        json += "\"b\":" + String(c.b);
        json += "}";
        request->send(200, "application/json", json);
    });

    // JSON-Rückgabe der verbleibenden Zeit
    server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){
        uint32_t remaining = timer.getRemainingTime();
        String json = "{";
        json += "\"remaining\":" + String(remaining);
        json += "}";
        request->send(200, "application/json", json);
    });

    server.on("/config", HTTP_POST, [](AsyncWebServerRequest *request) {}, 
    NULL,  // No file upload handler
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t, size_t) {
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, data, len);
    if (error) {
      request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }

    if (!doc.containsKey("duration")) {
      request->send(400, "application/json", "{\"error\":\"Missing 'duration'\"}");
      return;
    }

    uint32_t durationMin = doc["duration"];
    if (durationMin < 1 || durationMin > 120) {
      request->send(400, "application/json", "{\"error\":\"Out of range (1–120)\"}");
      return;
    }

    saveConfig(durationMin);
    request->send(200, "application/json", "{\"success\":true}");
    });

    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
        StaticJsonDocument<256> doc;
        File file = SPIFFS.open("/config.json", "r");
        if (!file) {
            request->send(500, "application/json", "{\"error\":\"Config not found\"}");
            return;
        }

        DeserializationError error = deserializeJson(doc, file);
        file.close();
        if (error) {
            request->send(500, "application/json", "{\"error\":\"Failed to parse config\"}");
            return;
        }   

        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });




  server.begin();
}


void saveConfig(uint32_t durationMinutes) {
    File file = SPIFFS.open("/config.json", FILE_WRITE);
    if (!file) {
        Serial.println("Konnte config.json nicht öffnen");
        return;
    }

    StaticJsonDocument<128> doc;
    doc["duration"] = durationMinutes;
    serializeJson(doc, file);
    file.close();
    Serial.println("Neue Dauer gespeichert: " + String(durationMinutes) + " Minuten");
}


