// -----------------------------
// Title: RGB Control
// -----------------------------
// Program Detail: 
// -----------------------------
// Purpose: The purpose of this program is to turn an LED on or off
// depending on the contents of a remote txt file (results.txt),
// as well as control the color of an RGB LED depending on the contents
// of a remote txt file (rgb.txt). A user controls the LED through an
// ON and OFF button on a webpage, and RGB values through a slider.
// Once a button is pressed, these values are read by the ESP8266,
// And the LEDs respond accordingly.

// Inputs: Button Input (D1)
// Outputs: RGB LED (D5, D6, D7), LED (D2)
// Date: 11/02/2025 12:04 PM PT
// Compiler: Arduino IDE 2.3.6
// Author: Zella Waltman
// Versions:
//          V1.0 = Original Code

// -----------------------------
// File Dependencies: WifiCred.h, button.h, LEDcontrol.h, RGBcontrol.h
// -----------------------------

#include <Arduino.h>
#include "button.h"
#include "WifiCred.h"
#include "LEDcontrol.h"
#include "RGBcontrol.h"

// -----------------------------
// Main Program
// -----------------------------

void setup() {

  Serial.begin(9600);

  // Initialization
  buttonInit();
  LED_Initialize();
  RGB_Initialize();

  checkWifi(); // Check Wifi Connection
  connectionDetails(); // Get Connection Info

  // Fetch values on boot
  fetchAndApply(); // Get LED State
  RGB_UpdateFromURL(); // Get RGB Values
}

void loop() {
  buttonPress(); // Check if Button is pressed, if pressed, execute

  // Short sleep
  delay(10);
}