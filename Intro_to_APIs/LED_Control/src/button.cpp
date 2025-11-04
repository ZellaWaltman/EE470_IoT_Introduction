//---------------------------------------------------------------
// Title: button.cpp
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: This is a file which stores the button press functions.

// Inputs: Button (D1)
// Outputs: LED value from results.txt
// Date: 11/01/2025 2:34 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code

//---------------------------------------------------------------
// File Dependencies: LEDcontrol.h, RGBcontrol.h
//---------------------------------------------------------------

#include <Arduino.h>
#include "PHPcontrol.h"

#define BTN D1

// initial button flag state
volatile bool g_btnEvent  = false;

// Interrupt
//---------------------------------------------------------------------------------

IRAM_ATTR void onButtonISR() {
    g_btnEvent = true;
}
//---------------------------------------------------------------------------------

// Initialize Button
void buttonInit() {
    pinMode(BTN, INPUT);
    attachInterrupt(digitalPinToInterrupt(BTN), onButtonISR, FALLING); // Button is an interrupt triggers after button press (falling edge)
}

// Check if Button = Pressed
void buttonPress() {
    static uint32_t lastBtn = 0;
    uint32_t current = millis();

    // If the button has been pressed
    if (g_btnEvent) {
        g_btnEvent = false; // Reset Interrupt flag
        if (current - lastBtn > 50) { // millis() debounce
            lastBtn = current;
            Serial.print("/nButton Pressed.");
            fetchAndApply(); // Get LED State
        }
    }
}
