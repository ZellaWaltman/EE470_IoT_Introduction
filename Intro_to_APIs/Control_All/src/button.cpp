#include <Arduino.h>
#include "control.h"

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
        if (current - lastBtn > 80) { // millis() debounce
            lastBtn = current;
            Serial.print("\nButton Pressed.\n");
            // 1) Fetch & apply from PHP (updates LED + RGB + lastLED/lastR/G/B)
            applyFromPHP();

            // 2) OPTIONAL: log the current state to Sheets with device time
            logToSheets();
        }
    }
}
