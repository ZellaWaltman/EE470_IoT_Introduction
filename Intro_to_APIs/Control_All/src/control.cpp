// 11/03/2025 2:20 PM

#include "WifiCred.h"
#include "getTimeAPI.h"
#include "control.h"

// LED Pin
#define LED D2

// RGB Pins
#define redPin D5
#define greenPin D6
#define bluePin D7

// PHP URLS
const char* RESULTS_URL = "https://zellawaltman.org/RGB_Control/results.txt";
const char* RGB_URL = "https://zellawaltman.org/RGB_Control/rgb.txt";

// Google Sheets URL
const char* SHEETS_POST_URL = "https://script.google.com/macros/s/AKfycbywJLt1Zn7bIln_j-aXDWwkK_KeOg9PAAqwRbec264DgzOvLk34ECfuJuF7-pjbp6qg/exec";

// Clamping for RGB
static uint8_t clamp255(int v){ return (uint8_t)(v < 0 ? 0 : (v > 255 ? 255 : v)); }

// Remembered States for Logging
static String  lastLED = "OFF";
static uint8_t lastR=0, lastG=0, lastB=0;

// ------------------------------------------------------------
// LED Functions
// ------------------------------------------------------------

void LED_Initialize() {
    pinMode(LED, OUTPUT);
}

void setLed(bool on) {
    digitalWrite(LED, on ? HIGH : LOW); // if user selects ON, then it is high. Else, LOW
}

// Initialize LED
void LEDInit() {
    pinMode(LED, OUTPUT);
    setLed(false); // start OFF
}

// ------------------------------------------------------------
// WiFi & PHP Functions
// ------------------------------------------------------------

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

// Fetching Functions:
// - - - - - - - - - - - - - - - - - - - - - - -

// Returns true and fills ledOut with "ON"/"OFF" after applying to the LED
static bool fetchLED_fromPHP(String& ledOut) {
  if (WiFi.status() != WL_CONNECTED) checkWifi();
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client; client.setInsecure();
  HTTPClient http;

  http.setTimeout(5000); // 5s

  String url = String(RESULTS_URL) + "?t=" + millis(); // cache buster
  if (!http.begin(client, url)) { http.end(); return false; }

  int code = http.GET();
  if (code == HTTP_CODE_OK) {
    String payload = http.getString();
    http.end();

    // apply + normalize
    String t = payload; t.trim(); t.toUpperCase();
    if (t == "ON")  setLed(true);
    if (t == "OFF") setLed(false);
    ledOut = t;
    lastLED = t; // Remember State
    return (t == "ON" || t == "OFF");
  }
  http.end();
  return false;
}

// Returns true and fills r,g,b after applying to RGB
static bool fetchRGB_fromPHP(uint8_t& r, uint8_t& g, uint8_t& b) {
  if (WiFi.status() != WL_CONNECTED) checkWifi();
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client; client.setInsecure();

  HTTPClient http;

  http.setTimeout(5000); // 5s

  String url = String(RGB_URL) + "?t=" + millis();
  if (!http.begin(client, url)) { http.end(); return false; }

  int code = http.GET();
  if (code == HTTP_CODE_OK) {
    String s = http.getString();
    http.end();

    if (parseCSV_RGB(s, r, g, b)) {
      setColor(r, g, b);
      lastR=r; lastG=g; lastB=b; // Remember State
      return true;
    }
  }
  http.end();
  return false;
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
// Control from PHP Function
// ------------------------------------------------------------

// Called when Apply button is pressed
bool applyFromPHP() {
  String ledState = "OFF";
  uint8_t r=0, g=0, b=0;

  // Fetch the latest values from your PHP pages
  bool okLED = fetchLED_fromPHP(ledState);    // applies to LED automatically
  bool okRGB = fetchRGB_fromPHP(r, g, b);     // applies to RGB automatically

  // Save the parsed values so the Log button can use them later
  if (okLED) lastLED = ledState;
  if (okRGB) { lastR = r; lastG = g; lastB = b; }

  // Return true if at least one succeeded
  return (okLED || okRGB);
}

// ------------------------------------------------------------
// Google Sheets Functions
// ------------------------------------------------------------

static bool postToSheets(const char* url, const String& body) {
  if (WiFi.status() != WL_CONNECTED) checkWifi();
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client; client.setInsecure();  // OK for class project
  HTTPClient http;
  http.setTimeout(5000); // 5s is plenty
  if (!http.begin(client, url)) { http.end(); return false; }
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int code = http.POST(body);
  http.end();
  return (code >= 200 && code < 300);
}

bool Log_to_Sheets() {
  String ts = getTime();                 // your existing function
  int dot = ts.indexOf('.');             // trim fractional seconds
  if (dot > 0) ts = ts.substring(0, dot); // -> "YYYY-MM-DDTHH:MM:SS"

  String body = "led=" + lastLED +
                "&r=" + String(lastR) +
                "&g=" + String(lastG) +
                "&b=" + String(lastB);

  if (ts.length()) body += "&ts=" + ts;  // Apps Script will use this

  return postToSheets(SHEETS_POST_URL, body);
}