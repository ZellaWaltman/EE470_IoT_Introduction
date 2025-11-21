//---------------------------------------------------------------
// Title: RGBcontrol.cpp
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: The purpose of this program is to control the color of an
// RGB LED depending on the contents of a remote txt file (rgb.txt)
// stored on a web server. A user controls the RGB values through a
// slider, and submits the values. These values are read by the ESP8266
// and adjust the RGB LED accordingly.

// Inputs: rgb.txt file
// Outputs: RGB LED (D5, D6, D7)
// Date: 11/01/2025 10:49 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code

//---------------------------------------------------------------
// File Dependencies: RGBcontrol.h, WifiCred.h
//---------------------------------------------------------------

#include "RGBcontrol.h"
#include "WifiCred.h"

// RGB Pins
#define redPin D5
#define greenPin D6
#define bluePin D7

// URL - Location of rgb.txt file
const char* RGB_URL = "https://zellawaltman.org/RGB_Control/rgb.txt";

// ------------------------------------------------------------
// RGB LED Functions
// ------------------------------------------------------------

// Initialize RGB LED
void RGB_Initialize() {
    // Defining pins as OUTPUT
    pinMode(redPin,  OUTPUT);              
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);

    setColor(255, 255, 255); // LED is initially white
}

// Write colors to each color pin of the LED
void setColor(int redValue, int greenValue,  int blueValue) {
    analogWrite(redPin, redValue);
    analogWrite(greenPin, greenValue);
    analogWrite(bluePin, blueValue);
}

// ------------------------------------------------------------
// WiFi & PHP Functions
// ------------------------------------------------------------

// Connect to site & get rgb.txt
// Read & seperate the RGB values
// Apply the state based on txt
bool RGB_UpdateFromURL() {
  // Make sure WiFi is connected
  if (WiFi.status() != WL_CONNECTED) checkWifi();
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  // Cache buster - force to get a fresh copy of rgb.txt
  String url = String(RGB_URL) + "?t=" + String(millis());

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

    // Find commas - if less than two commas, INVALID
    int c1 = s.indexOf(','), c2 = s.indexOf(',', c1+1);
    if (c1 > 0 && c2 > c1) {
      // Clamp numeric values to 0-255 range, 0 if less than 0, 255 if greater than 255
      // Red Value = before first comma. Get rid of everything else
      uint8_t r = constrain(s.substring(0, c1).toInt(), 0, 255);
      // Green Value = between commas. Get rid of everything else
      uint8_t g = constrain(s.substring(c1+1, c2).toInt(), 0, 255);
      // Blue Value = after second comma. Get rid of everything else
      uint8_t b = constrain(s.substring(c2+1).toInt(), 0, 255);
      setColor(r, g, b);
      return true;
    }
  } // Failure Case
  http.end();
  return false;
}