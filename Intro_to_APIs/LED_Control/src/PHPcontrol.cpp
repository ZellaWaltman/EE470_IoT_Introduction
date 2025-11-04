//---------------------------------------------------------------
// Title: PHPcontrol.cpp
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: The purpose of this program is to turn an LED on or off
// depending on the contents of a remote txt file (results.txt) stored
// on a web server. A user presses on or off on a website, and the LED
// will respond accordingly.

// Inputs: results.txt file
// Outputs: LED (D2)
// Date: 11/01/2025 3:04 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code

//---------------------------------------------------------------
// File Dependencies: PHPcontrol.h, WifiCred.h
//---------------------------------------------------------------

#include "PHPcontrol.h"
#include "WifiCred.h"

#define LED D2

// URL - Location of results.txt file
const char* RESULTS_URL = "https://zellawaltman.org/RGB_Control/results.txt";

// Initialize LED
void LEDInit() {
    pinMode(LED, OUTPUT);
    setLed(false); // start OFF
}

// Turn LED on or off
void setLed(bool on) {
    digitalWrite(LED, on ? HIGH : LOW); // if user selects ON, then it is high. Else, LOW
}

// ------------------------------------------------------------
// WiFi & PHP Functions
// ------------------------------------------------------------

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
   // Trim & Modify results.txt for proper read
  String t = txtIn;
  t.trim();
  t.toUpperCase();

  if (t == "ON") {
    setLed(true); // results.txt = ON -> LED = ON
  } else if (t == "OFF") {
    setLed(false); // results.txt = OFF -> LED = OFF
  }
}

// Connect to site & get results.txt
// Read & convert the txt body
// Apply the state based on txt
bool fetchAndApply() {
  ensureWiFi(); // Make Sure Wifi = Connected
  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;
  WiFiClientSecure client;
  client.setInsecure();

  // Cache buster - force to get a fresh copy of results.txt
  String url = String(RESULTS_URL) + "?t=" + String(millis());

  // Initial HTTP Connect
  if (!http.begin(client, url)) {
    http.end();
    return false;
  }

  // GET results.txt
  int code = http.GET();
  if (code == HTTP_CODE_OK) { // If request succeeded
    String payload = http.getString(); // get txt of results.txt
    applyStateFromText(payload); // Convert to readable command "ON" or "OFF"
    http.end(); // Close Connection
    return true;
  } else { // Failure Case
    http.end();
    return false;
  }
}