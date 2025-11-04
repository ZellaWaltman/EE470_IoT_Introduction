//---------------------------------------------------------------
// Title: button.h
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: This is a header file which stores the button functions.
// Inputs: none
// Outputs: none
// Date: 11/01/2025 5:57 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code
//---------------------------------------------------------------
// File Dependencies: none
//---------------------------------------------------------------

#ifndef LEDCONTROL_H
#define LEDCONTROL_H 

#include <Arduino.h>

// Function Declarations
// ------------------------------
void LED_Initialize();
void setLed(bool on);
void applyStateFromText(const String& txtIn);
static bool fetchLED_fromPHP(String& ledOut); 

#endif // LEDCONTROL_H