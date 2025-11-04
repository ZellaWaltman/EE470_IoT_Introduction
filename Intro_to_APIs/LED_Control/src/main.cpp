//---------------------------------------------------------------
// Title: PHPcontrol.cpp
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: The purpose of this program is to turn an LED on or off
// depending on the contents of a remote txt file (results.txt) stored
// on a web server. A user presses on or off on a website, and once a
// button connected to the ESP8266 is pressed, these values are read
// by the ESP, and the LED responds accordingly.

// Inputs: Button (D1)
// Outputs: LED (D2)
// Date: 11/01/2025 7:12 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code

//---------------------------------------------------------------
// File Dependencies: button.h, PHPcontrol.h, WifiCred.h
//---------------------------------------------------------------

#include <Arduino.h>
#include "WifiCred.h"
#include "button.h"
#include "PHPcontrol.h"

// -----------------------------
// Main Program
// -----------------------------

void setup() {

  Serial.begin(9600);

  // Initialization
  buttonInit();
  LEDInit();

  checkWifi(); // Check Wifi Connection
  connectionDetails(); // Get Connection Info

  // Fetch value on boot
  fetchAndApply();
}

void loop() {
  buttonPress(); // Check if Button is pressed, if pressed, execute

  // Short sleep
  delay(10);
}