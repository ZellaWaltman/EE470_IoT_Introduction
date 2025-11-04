//---------------------------------------------------------------
// Title: control.cpp
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: The purpose of this program is to turn an LED on or off
// depending on the contents of a remote txt file (results.txt),
// as well as control the color of an RGB LED depending on the contents
// of a remote txt file (rgb.txt). A user controls the LED through an
// ON and OFF button on a webpage, and RGB values through a slider.
// After the LEDs respond, the data is sent to a Google Spreadsheet
// with a timestamp from timeapi.io.

// Inputs: results.txt file, rgb.txt file
// Outputs: LED (D2) RGB LED (D5, D6, D7)
// Date: 11/03/2025 2:20 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code

//---------------------------------------------------------------
// File Dependencies: control.h, WifiCred.h, getTimeAPI.h
//---------------------------------------------------------------

#include "WifiCred.h"
#include "getTimeAPI.h"
#include "control.h"

// LED Pin
#define LED D2

// RGB Pins
#define redPin D5
#define greenPin D6
#define bluePin D7

// PHP URLS - Location of the results.txt and rgb.txt files
const char* RESULTS_URL = "https://zellawaltman.org/RGB_Control/results.txt";
const char* RGB_URL = "https://zellawaltman.org/RGB_Control/rgb.txt";

// Google Sheets URL
const char* SHEETS_POST_URL = "https://script.google.com/macros/s/AKfycbywJLt1Zn7bIln_j-aXDWwkK_KeOg9PAAqwRbec264DgzOvLk34ECfuJuF7-pjbp6qg/exec";

// Clamp numeric values to 0-255 range for RGB, 0 if less than 0, 255 if greater than 255
static uint8_t clamp255(int v){ return (uint8_t)(v < 0 ? 0 : (v > 255 ? 255 : v)); }

// Remembered States for Logging to Spreadsheet
static String  lastLED = "OFF";
static uint8_t lastR=0, lastG=0, lastB=0;

// ------------------------------------------------------------
// LED Functions
// ------------------------------------------------------------

// Initialize LED
void LED_Initialize() {
    pinMode(LED, OUTPUT);
}

// Turn LED on or off
void setLed(bool on) {
    digitalWrite(LED, on ? HIGH : LOW); // if user selects ON, then it is high. Else, LOW
}

// Initialize LED
void LEDInit() {
    pinMode(LED, OUTPUT);
    setLed(false); // start OFF
}

// ------------------------------------------------------------
// RGB LED Functions
// ------------------------------------------------------------

void RGB_Initialize() {
    // Defining pins as OUTPUT
    pinMode(redPin,  OUTPUT);              
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    analogWriteRange(255); // Maps to 255
    setColor(255, 255, 255); // LED is initially white
}

void setColor(int redValue, int greenValue,  int blueValue) {
    analogWrite(redPin, redValue);
    analogWrite(greenPin, greenValue);
    analogWrite(bluePin, blueValue);
}

// "R,G,B" parser with trimming and clamping
static bool parseCSV_RGB(const String& s, uint8_t &r, uint8_t &g, uint8_t &b){
  int c1 = s.indexOf(',');
  if (c1 < 0) return false;
  int c2 = s.indexOf(',', c1+1);
  if (c2 < 0) return false;

  String rs = s.substring(0, c1);       rs.trim();
  String gs = s.substring(c1+1, c2);    gs.trim();
  String bs = s.substring(c2+1);        bs.trim();

  if (!rs.length() || !gs.length() || !bs.length()) return false;

  r = clamp255(rs.toInt());
  g = clamp255(gs.toInt());
  b = clamp255(bs.toInt());
  return true;
}

// ------------------------------------------------------------
// WiFi & PHP Functions
// ------------------------------------------------------------

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

// Fetching Functions:
// - - - - - - - - - - - - - - - - - - - - - - -

// Connect to site & get results.txt
// Read & convert the txt body
// Apply the state based on txt
static bool fetchLED_fromPHP(String& ledOut) {
  // Make Sure Wifi = Connected
  if (WiFi.status() != WL_CONNECTED) checkWifi();
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  http.setTimeout(5000); // 5s

  // Cache buster - force to get a fresh copy of results.txt
  String url = String(RESULTS_URL) + "?t=" + millis();

  // Initial HTTP Connect
  if (!http.begin(client, url)) {
    http.end();
    return false; 
  }

  // GET results.txt
  int code = http.GET();
  if (code == HTTP_CODE_OK) { // If request succeeded
    String payload = http.getString(); // get txt of results.txt
    http.end(); // Close Connection

    // Convert to readable command "ON" or "OFF"
    applyStateFromText(payload);
    ledOut = payload; 
    lastLED = payload; // Remember State For Logging
  }
  http.end(); // Failure Case
  return false;
}

// Returns true and fills r,g,b after applying to RGB
static bool fetchRGB_fromPHP(uint8_t& r, uint8_t& g, uint8_t& b) {
  // Make sure WiFi is connected
  if (WiFi.status() != WL_CONNECTED) checkWifi();
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  http.setTimeout(5000); // 5s

  // Cache buster - force to get a fresh copy of rgb.txt
  String url = String(RGB_URL) + "?t=" + millis();

  // Initial HTTP Connect
  if (!http.begin(client, url)) { 
    http.end();
    return false;
  }

  // GET rgb.txt
  int code = http.GET();
  if (code == HTTP_CODE_OK) { // If request succeeded
    String s = http.getString(); // get txt of rgb.txt
    http.end(); // Close Connection

    if (parseCSV_RGB(s, r, g, b)) { // Parse RGB values in rgb.txt
      setColor(r, g, b); // Set RGB values for RGB LED
      lastR=r; lastG=g; lastB=b; // Remember State for Logging
      return true;
    }
  }
  http.end(); // Failure Case
  return false;
}

// Create Temp Storage Variables for Logging to Sheets
bool applyFromPHP() {
  String ledState = "OFF";
  uint8_t r=0, g=0, b=0;

  // Get latest values from PHP pages
  bool okLED = fetchLED_fromPHP(ledState); 
  bool okRGB = fetchRGB_fromPHP(r, g, b);

  // Save parsed values for Sheets
  if (okLED) lastLED = ledState;
  if (okRGB) { lastR = r; lastG = g; lastB = b; }

  // If at least one = good, return true
  return (okLED || okRGB);
}

// ------------------------------------------------------------
// Google Sheets Functions
// ------------------------------------------------------------

//
bool logToSheets() {
  String ts = getTime(); // Get Time
  int dot = ts.indexOf('.'); // trim microseconds
  if (dot > 0) ts = ts.substring(0, dot); // Format = "YYYY-MM-DDTHH:MM:SS"

  // Stored Data Log Variables for Sheets
  String body = "led=" + lastLED +
                "&r=" + String(lastR) +
                "&g=" + String(lastG) +
                "&b=" + String(lastB);

  // Timestamp variable for Apps Script
  if (ts.length()) body += "&ts=" + ts;

  // Post data to Google Spreadsheet
  return postToSheets(SHEETS_POST_URL, body);
}

// Post data to Google Spreadsheet
static bool postToSheets(const char* url, const String& body) {
  // Make sure WiFi is connected
  if (WiFi.status() != WL_CONNECTED) checkWifi();
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  http.setTimeout(5000); // 5s

  // Initial HTTP Connect
  if (!http.begin(client, url)) { 
    http.end();
    return false;
  }

  // Tell server request body looks like normal HTML form submission
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // send POST request w/ data
  int code = http.POST(body);

  http.end(); // Close Connection

  return (code >= 200 && code < 300); // TRUE w/ success code, false w/ failure
}