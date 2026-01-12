#include <ArduinoJson.h>
#include <LittleFS.h>
#include "header/led.h"
#include "header/scenery.h"

const char* saveFileName = "/save.json";

namespace SAVE {
    bool busy = false;

    // ================= INIT =================
    void init() {
        if (LittleFS.exists(saveFileName)) return;

        JsonDocument doc;
        // Crée "last" array
        JsonArray lastArray = doc["last"].to<JsonArray>();
        lastArray.add(0); // r
        lastArray.add(0); // g
        lastArray.add(0); // b
        lastArray.add(2); // scene

        // Crée "save" array vide
        doc["save"].to<JsonArray>();

        File file = LittleFS.open(saveFileName, "w");
        if (file) {
            serializeJson(doc, file);
            file.close();
        }
    }

    // ================= SEND LAST STATE =================
    void sendLastState() {
        if (busy) return;
        busy = true;

        File file = LittleFS.open(saveFileName, "r");
        if (!file) { busy = false; return; }

        JsonDocument doc;
        if (deserializeJson(doc, file) != DeserializationError::Ok) {
            file.close();
            busy = false;
            return;
        }
        file.close();

        JsonArray lastArray = doc["last"].as<JsonArray>();
        lastArray[0] = LED::pwmRed;
        lastArray[1] = LED::pwmGreen;
        lastArray[2] = LED::pwmBlue;
        lastArray[3] = getSceneryIndex();

        file = LittleFS.open(saveFileName, "w");
        if (file) {
            serializeJson(doc, file);
            file.close();
        }

        busy = false;
    }

    // ================= CHECK EXISTENCE =================
    bool exist() {
        return LittleFS.exists(saveFileName);
    }

    // ================= OPEN READ =================
    File openRead() {
        return LittleFS.open(saveFileName, "r");
    }

    // ================= SAVE NEW COLOR =================
    const char* saveColors(const String& newColor) {
        if (busy) return "FS busy";
        busy = true;

        File file = openRead();
        if (!file) {
            busy = false;
            return "failed to open file";
        }

        JsonDocument doc;
        if (deserializeJson(doc, file) != DeserializationError::Ok) {
            file.close();
            busy = false;
            return "JSON error";
        }
        file.close();

        JsonArray saveArray = doc["save"].as<JsonArray>();
        saveArray.add(newColor);

        file = LittleFS.open(saveFileName, "w");
        if (file) {
            serializeJson(doc, file);
            file.close();
        }

        busy = false;
        return "saved!";
    }

} // namespace SAVE
