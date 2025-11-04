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

#ifndef PHPCONTROL_H
#define PHPCONTROL_H 

#include <Arduino.h>

// Function Declarations
// ------------------------------
void setLed(bool on);
void LEDInit();
void ensureWiFi();
void applyStateFromText(const String& txtIn);
bool fetchAndApply(); 

#endif // PHPCONTROL_H