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

//------------------------------4
// Definitions & Variables
//------------------------------

// Define Pins
//------------------------------
#define redPin D5 
#define greenPin D6 
#define bluePin D7 
#define LDR_Pin A0 // LDR connected to Analog pin A0
#define BUZZER D1 // Buzzer Pin (D1)

// ADC & Voltage Divider Constants
//----------------------------------
#define VIN 3.3 // 3.3 V Power Voltage

// Function Declarations
//------------------------------
void setColor(int redValue, int greenValue,  int blueValue);

// -----------------------------
// Main Program
// -----------------------------

void setup() {

  // Defining pins as OUTPUT
  pinMode(redPin,  OUTPUT);              
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(BUZZER,  OUTPUT);  

  setColor(255, 255, 255); // LED is initially white

  Serial.begin(9600); // Set Baud Rate (9600)
}

void  loop() {
    setColor(255, 0, 0);
}

void setColor(int redValue, int greenValue,  int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin,  greenValue);
  analogWrite(bluePin, blueValue);
}