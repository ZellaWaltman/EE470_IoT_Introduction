//---------------------------------------------------------------
// Title: Control All
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: The purpose of this program is to turn an LED on or off
// depending on the contents of a remote txt file (results.txt),
// as well as control the color of an RGB LED depending on the contents
// of a remote txt file (rgb.txt). A user controls the LED through an
// ON and OFF button on a webpage, and RGB values through a slider.
// Once a button is pressed, these values are read by the ESP8266,
// And the LEDs respond accordingly. After the LEDs respond, the data
// is sent to a Google Spreadsheet with a timestamp from timeapi.io.

// Inputs: Button (D1)
// Outputs: LED (D2) RGB LED (D5, D6, D7)
// Date: 11/03/2025 3:43 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code

//---------------------------------------------------------------
// File Dependencies: control.h, WifiCred.h, button.h
//---------------------------------------------------------------

#include <Arduino.h>
#include "button.h"
#include "WifiCred.h"
#include "control.h"

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

  // Fetch LED State and RGB Values on boot
  applyFromPHP();
}

void loop() {
  buttonPress(); // Check if Button is pressed, if pressed, execute

  // Short sleep to reduce CPU churn
  delay(5);
}