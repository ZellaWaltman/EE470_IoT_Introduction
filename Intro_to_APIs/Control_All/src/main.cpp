// -----------------------------
// Title: LUX Meter
// -----------------------------
// Program Detail: 
// -----------------------------
// Purpose: This program is a LUX meter. When the LUX value is below the threshold (300 LUX),
// a buzzer will sound, and an RGB LED will be white. Past the threshold (300 LUX), the buzzer
// will turn off, and the RGB LED will turn more Red the higher the LUX value becomes. The
// LUX value will be printed to the terminal every 0.5s. If the enters 'B' or 'b' into the
// Terminal, the buzzer will buzz for 5 seconds, to ensure it is working correctly.

// Inputs: LDR pin analog reading (A0), terminal input ('b' or 'B')
// Outputs: RGB LED (D5, D6, D7), Buzzer (D1), Terminal Output LUX Reading
// Date: 09/19/2025 12:04 PM PT
// Compiler: Arduino IDE 2.3.6
// Author: Zella Waltman
// Versions:
//          V1.0 = Original Code

// -----------------------------
// File Dependencies: None
// -----------------------------

#include <Arduino.h>
#include "buttons.h"
#include "WifiCred.h"
#include "control.h"

//------------------------------4
// Definitions & Variables
//------------------------------

// ADC & Voltage Divider Constants
//----------------------------------
#define VIN 3.3 // 3.3 V Power Voltage


// -----------------------------
// Main Program
// -----------------------------

void setup() {

  Serial.begin(9600);

  buttonInit();
  LED_Initialize();
  RGB_Initialize();

  checkWifi(); // Check Wifi Connection
  connectionDetails(); // Get Connection Info

  // Do one fetch at boot
  syncFromPHP_andLogToSheets();
}

void loop() {
  buttonAPress(); // Check if Button is pressed

  // Short sleep to reduce CPU churn
  delay(10);
}