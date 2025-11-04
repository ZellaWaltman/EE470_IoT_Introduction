// 11/01/2025 6:05 PM

#include "PHPcontrol.h"
#include "WifiCred.h"

#define LED D2

const char* RESULTS_URL = "https://zellawaltman.org/RGB_Control/results.txt";

// LED ON or OFF
void setLed(bool on) {
    digitalWrite(LED, on ? HIGH : LOW); // if user selects ON, then it is high. Else, LOW
}

// Initialize LED
void LEDInit() {
    pinMode(LED, OUTPUT);
    setLed(false); // start OFF
}

// Ensure WiFi is connected
void ensureWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(250);
  }
}

// Get result.txt and grab the state from it
void applyStateFromText(const String& txtIn) {
  String t = txtIn;
  t.trim();
  t.toUpperCase();

  if (t == "ON") {
    setLed(true);
  } else if (t == "OFF") {
    setLed(false);
  }
  // silently ignore anything else
}

bool fetchAndApply() {
  ensureWiFi();
  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure(); // for https without cert. For production, install a root CA.

  // add a cache buster query to avoid CDN/browser caching if any
  String url = String(RESULTS_URL) + "?t=" + String(millis());

  if (!http.begin(client, url)) {
    http.end();
    return false;
  }

  int code = http.GET();
  if (code == HTTP_CODE_OK) {
    String payload = http.getString();
    applyStateFromText(payload);
    http.end();
    return true;
  } else {
    http.end();
    return false;
  }
}