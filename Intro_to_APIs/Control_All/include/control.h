//---------------------------------------------------------------
// Title: LEDcontrol.h
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: This is a header file which stores the LED and RGB control functions,
// as well as PHP and Google Sheets functions.

// Inputs: none
// Outputs: none
// Date: 11/03/2025 3:10 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code

//---------------------------------------------------------------
// File Dependencies: none
//---------------------------------------------------------------

#ifndef CONTROL_H
#define CONTROL_H 

#include <Arduino.h>

// LED Function Declarations
// ------------------------------
void LED_Initialize();
void setLed(bool on);
void applyStateFromText(const String& txtIn);

// RGB LED Function Declarations
// ------------------------------
static bool parseCSV_RGB(const String& s, uint8_t &r, uint8_t &g, uint8_t &b);
void RGB_Initialize();
void setColor(int redValue, int greenValue,  int blueValue); 

// PHP Function Declarations
// ------------------------------------
static bool fetchLED_fromPHP(String& ledOut); 
static bool fetchRGB_fromPHP(uint8_t& r, uint8_t& g, uint8_t& b); 
bool applyFromPHP();

// Google Sheets Function Declarations
// ------------------------------------
bool logToSheets();
static bool postToSheets(const char* url, const String& body);

#endif // CONTROL_H