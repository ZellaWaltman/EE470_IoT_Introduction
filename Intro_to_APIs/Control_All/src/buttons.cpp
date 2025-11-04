#include <Arduino.h>
#include "control.h"

#define LED_BTN D1
#define SHEETS_BTN D4

// initial button flag state
volatile bool g_ledBTN  = false;
volatile bool g_sheetsBTN  = false;

// Interrupt
//---------------------------------------------------------------------------------

IRAM_ATTR void onLEDISR() {
    g_ledBTN = true;
}

IRAM_ATTR void onSHEETSISR() {
    g_sheetsBTN = true;
}

//---------------------------------------------------------------------------------

// Initialize Button
void buttonInit() {

    // LED Control Button
    pinMode(LED_BTN, INPUT);
    attachInterrupt(digitalPinToInterrupt(LED_BTN), onLEDISR, FALLING); // Button is an interrupt triggers after button press (falling edge)

    // Google Sheets Log Button
    pinMode(SHEETS_BTN, INPUT);
    attachInterrupt(digitalPinToInterrupt(SHEETS_BTN), onSHEETSISR, FALLING); // Button is an interrupt triggers after button press (falling edge)

}

// Check if Button = Pressed
void buttonAPress() {
    static uint32_t lastBTNA = 0;
    uint32_t current = millis();

    // If the button has been pressed
    if (g_ledBTN) {
        g_ledBTN = false; // Reset Interrupt flag
        if (current - lastBTNA > 50) { // millis() debounce
            lastBTNA = current;
            Serial.print("\nControl Button Pressed.");
            applyFromPHP();  
        }
    }
}

void serviceLogButton() {
  static uint32_t lastBTNB=0;
  uint32_t now = millis();

    // If the button has been pressed
    if (g_sheetsBTN) {
        g_sheetsBTN = false; // Reset Interrupt flag
        if (now - lastBTNB > 50) { // millis() debounce
            lastBTNB = now;
            Serial.print("\nLOG Button Pressed. Sending Info to Google Sheets.");
            Log_to_Sheets();
        }
    }
}