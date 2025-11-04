//---------------------------------------------------------------
// Title: LEDcontrol.h
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: This is a header file which stores the LED control functions.

// Inputs: none
// Outputs: none
// Date: 11/01/2025 10:47 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code

//---------------------------------------------------------------
// File Dependencies: none
//---------------------------------------------------------------


#ifndef RGBCONTROL_H
#define RGBCONTROL_H

#include <Arduino.h>

void RGB_Initialize();
void setColor(int redValue, int greenValue,  int blueValue); 
bool RGB_UpdateFromURL(); 

#endif // RGBCONTROL_H