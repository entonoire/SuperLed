#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#include "header/scenery.h"
#include "header/led.h"
#include "header/button.h"
#include "header/pinRegistry.h"
#include "header/file.h"

// ====================== WIFI ======================

const char* ssid     = "Box8_De_Combat";
const char* password = "puxlyrnyf5pkcjbsngwg";

ESP8266WebServer server(80);

// ====================== HANDLERS ======================

void handleLoad() {
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (SAVE::busy) {
        server.send(503, "application/json", "{}");
        return;
    }

    SAVE::busy = true;

    File file = SAVE::openRead();
    if (!file || file.size() == 0) {
        SAVE::busy = false;
        server.send(200, "application/json", "{}");
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();

    SAVE::busy = false;

    if (err) {
        server.send(200, "application/json", "{}");
        return;
    }

    String out;
    serializeJson(doc, out);
    server.send(200, "application/json", out);
}

// ====================== ROUTES ======================

void setupRequests() {

    // --- SET RGB / SCENE ---
    server.on("/set", HTTP_GET, []() {

        if (server.hasArg("r")) LED::pwmRed   = server.arg("r").toInt();
        if (server.hasArg("g")) LED::pwmGreen = server.arg("g").toInt();
        if (server.hasArg("b")) LED::pwmBlue  = server.arg("b").toInt();

        if (server.hasArg("scene")) {
            int s = server.arg("scene").toInt();
            if (s != getSceneryIndex()) {
                setScenery(static_cast<Scenery>(s));
            }
        }

        BTN::sendUpdate();
        LED_ColorRGB();

        if (!SAVE::busy) {
            SAVE::busy = true;
            SAVE::sendLastState();
            SAVE::busy = false;
        }

        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200);
    });

    // --- GET CURRENT STATE ---
    server.on("/get", HTTP_GET, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");

        char buff[32];
        sprintf(
            buff,
            "%d,%d,%d,%d",
            LED::pwmRed,
            LED::pwmGreen,
            LED::pwmBlue,
            getSceneryIndex()
        );

        server.send(200, "text/plain", buff);
    });

    // --- LOAD JSON ---
    server.on("/load", HTTP_GET, handleLoad);

    // --- SAVE COLOR ---
    server.on("/save", HTTP_GET, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");

        if (!server.hasArg("content")) {
            server.send(400, "text/plain", "Missing content");
            return;
        }

        if (SAVE::busy) {
            server.send(503, "text/plain", "FS busy");
            return;
        }

        SAVE::busy = true;
        const char* res = SAVE::saveColors(server.arg("content"));
        SAVE::busy = false;

        server.send(200, "text/plain", res);
    });

    // --- FAVICON (important) ---
    server.on("/favicon.ico", HTTP_GET, []() {
        server.send(204);
    });
}

// ====================== WEB INIT ======================

void loadIndex() {
    setupRequests();

    // Static file server (index.html, js, css, etc.)
    server.serveStatic("/", LittleFS, "/");

    server.begin();
}

// ====================== SETUP ======================

void setupWeb() {

    pinMode(LED_RED_PIN, OUTPUT);
    digitalWrite(LED_RED_PIN, HIGH);
    BTN::ledOff();

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(LED_RED_PIN, LOW);
        BTN::ledRGB(255, 255, 0);
        delay(300);
        digitalWrite(LED_RED_PIN, HIGH);
        BTN::ledOff();
        delay(300);
    }

    digitalWrite(LED_RED_PIN, LOW);
    BTN::ledRGB(255, 0, 0);

    if (!LittleFS.begin()) {
        BTN::ledRGB(255, 0, 255); // FS error
        return;
    }

    loadIndex();
}

// ====================== LOOP ======================

void handle() {
    server.handleClient();
}
