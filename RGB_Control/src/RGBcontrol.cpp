// 11/01/2025 10:47 PM

#include "RGBcontrol.h"
#include "WifiCred.h"

#define redPin D5
#define greenPin D6
#define bluePin D7

const char* RGB_URL = "https://zellawaltman.org/RGB_Control/rgb.txt";

static uint8_t clamp255(int v){ return (uint8_t)(v < 0 ? 0 : (v > 255 ? 255 : v)); }

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
// RGB LED Functions
// ------------------------------------------------------------

void RGB_Initialize() {
    // Defining pins as OUTPUT
    pinMode(redPin,  OUTPUT);              
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    setColor(255, 255, 255); // LED is initially white
}

void setColor(int redValue, int greenValue,  int blueValue) {
    analogWrite(redPin, redValue);
    analogWrite(greenPin, greenValue);
    analogWrite(bluePin, blueValue);
}

bool RGB_UpdateFromURL() {
  if (WiFi.status() != WL_CONNECTED) checkWifi();
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client; client.setInsecure();
  HTTPClient http;

  String url = String(RGB_URL) + "?t=" + String(millis());
  if (!http.begin(client, url)) { http.end(); return false; }

  int code = http.GET();
  if (code == HTTP_CODE_OK) {
    String s = http.getString();
    http.end();

    int c1 = s.indexOf(','), c2 = s.indexOf(',', c1+1);
    if (c1 > 0 && c2 > c1) {
      uint8_t r = constrain(s.substring(0, c1).toInt(), 0, 255);
      uint8_t g = constrain(s.substring(c1+1, c2).toInt(), 0, 255);
      uint8_t b = constrain(s.substring(c2+1).toInt(), 0, 255);
      setColor(r, g, b);
      return true;
    }
  }
  http.end();
  return false;
}