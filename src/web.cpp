#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include "header/scenery.h"
#include "header/led.h"
#include "header/button.h"

const char* saveFileName = "/save.txt";
const char* ssid = "Freebox_de_combat";
const char* password = "KKtfzshRjF";

ESP8266WebServer server(80);

void handleLoad() {
    server.sendHeader("Access-Control-Allow-Origin", "*");

    if (!LittleFS.exists(saveFileName)) {
        server.send(200, "text/plain", ""); // fichier vide
        return;
    }

    File file = LittleFS.open(saveFileName, "r");
    if (!file) {
        server.send(500, "text/plain", "Failed to open file");
        return;
    }

    // Utiliser server.streamFile pour envoyer le fichier directement
    server.streamFile(file, "text/plain");

    file.close();
}

const char* saveColors(const String& newColor) {
    File file = LittleFS.open(saveFileName, "a"); // append et crée si inexistant
    if (!file) {
        return "failed to open file";
    }

    String colorToSave = newColor;
    if (!newColor.endsWith(";")) colorToSave += ";";

    file.print(colorToSave); // ex: "rgb(255,0,0);"
    file.close();
    return "saved!";
}

void setupRequests() {
    server.on("/set", HTTP_GET, []() {
        // URL: /set?r=255&g=0&b=0&scene=1
        if (server.hasArg("r")) {
            LED::pwmRed = server.arg("r").toInt();
        }
        if (server.hasArg("g")) {
            LED::pwmGreen = server.arg("g").toInt();
        }
        if (server.hasArg("b")) {
            LED::pwmBlue = server.arg("b").toInt();
        }
        if (server.hasArg("scene")) {
            int index = server.arg("scene").toInt();
            if (index != getSceneryIndex()) {
                Scenery scene = static_cast<Scenery>(index);
                setScenery(scene);
            }
        }

        BTN::sendUpdate();
        LED_ColorRGB();
        server.sendHeader("Access-Control-Allow-Origin","*");
        server.send(200);
    });

    server.on("/get", HTTP_GET, []() {
        server.sendHeader("Access-Control-Allow-Origin","*");
        char buff[32];
        sprintf(buff, "%d,%d,%d,%d", LED::pwmRed, LED::pwmGreen, LED::pwmBlue, getSceneryIndex());
        server.send(200, "text/plain", buff);
    });

    server.on("/load", HTTP_GET, handleLoad);

    server.on("/save", HTTP_GET, []() {
        if (!server.hasArg("content")) {
            server.sendHeader("Access-Control-Allow-Origin","*");
            server.send(400, "text/plain", "Missing content");
            return;
        }
        server.sendHeader("Access-Control-Allow-Origin","*");
        server.send(200, "text/plain", saveColors(server.arg("content")));
    });
}

void loadIndex() {
    server.on("/", HTTP_GET, []() {
        File file = LittleFS.open("/index.html", "r");
        if (!file) {
            server.send(404, "text/plain", "index.html not found");
            return;
        }
        server.streamFile(file, "text/html");
        file.close();
    });

    setupRequests();
    server.serveStatic("/", LittleFS, "/");
    server.begin();
}

void setupWeb() {
    pinMode(D7, OUTPUT);
    digitalWrite(D7, HIGH); // LED OFF
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(D7, LOW); // blink
        delay(300);
        digitalWrite(D7, HIGH);
        delay(300);
    }

    if (WiFi.status() != WL_CONNECTED) {
        // failed
        return;
    }

    digitalWrite(D7, LOW); // LED ON

    if (!LittleFS.begin()) {
        return;
    }

    loadIndex();
}

void handle() {
    server.handleClient();
}
