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
#define R 10000 // Voltage divider Resistor value

float LDR_Val; // Analog Value from Photoresistor
float LUX; // Lux Value

// Buzzer - uses millis() (non-blocking)
//------------------------------------------
bool buzzing;
unsigned long buzzStart; // Buzzer Start time (millis())
unsigned long buzzPeriod = 5000; // 5s timing

// Printing LUX timing (millis())
//------------------------------------------
unsigned long printEnd = 0; // Buzzer End time (millis())
unsigned long printPeriod = 500; // 0.5s timing

//------------------------------
// Function Declarations
//------------------------------
float conversion(float LDR_raw);
void buzzerCheck();
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

  // Read RAW LDR & Convert to LUX
  LDR_Val = analogRead(LDR_Pin);
  LUX = conversion(LDR_Val);

  // If user presses B, buzzer plays for 5s
  buzzerCheck();

  // Print LUX (every .5 s)
  if (millis() - printEnd > printPeriod) { // if current time has surpassed 0.5s
    Serial.print("LUX value = ");
    Serial.print(LUX);
    Serial.println(" Lux"); 
    printEnd = millis(); // Mark start time
  }
  
  if (LUX >= 300.0) {

    // Map LDR values (0-1023) to RGB values (0-255)
    int fading = map (LDR_Val, 0, 1023, 0, 255);
  
    int newGreen = 255 - fading;
    int newBlue = 255 - fading;
    
    setColor(255, newGreen, newBlue);
  }
  else {
    setColor(255, 255, 255); // LED is white
  }

      // If NOT already Buzzing: OVERRIDE "OFF" FROM THRESHOLD
    if (buzzing == false) { // if not buzzing already from manual test
      if (LUX < 300.0) {
        digitalWrite(BUZZER, HIGH);
      }
      else {
        digitalWrite(BUZZER, LOW); // if threshold HIGH, buzzer is off
      }
    }

}

float conversion(float LDR_raw) {
  
  // Convert from analog to voltage (LDR Analog Output * Resolution)
  float Vout = float(LDR_raw) * (VIN / float(1023));

  //Serial.print("Vout = ");
  //Serial.print(Vout, 3); // 3 decimal places
  //Serial.println(" V");
  
  // Convert Vout to LUX:
  // Values determined during Calibration (EXCEL data)
  float lux = 9.6549*exp(Vout*1.7936);

  return lux; // return lux approximation
}

void buzzerCheck() {
  while (Serial.available() > 0) { // If user types in console
    char entry = (char)Serial.read(); // Get char from user input (B)
    Serial.println(entry); // Print what user typed in console

    if (entry == 'b' || entry == 'B') { // If user types 'b' or 'B' turn buzzer on for 5s
        digitalWrite(BUZZER, HIGH); // Turn on buzzer
        buzzing = true;
        buzzStart = millis(); // Mark start time (ms)
        Serial.println("Manual Buzzer Test (5s)");
      }
      
  }

  // if Buzzer is on and has been past 5 seconds 
  if (buzzing == true && (millis() - buzzStart >= buzzPeriod)){ // Current time - start time >= 5s
    digitalWrite(BUZZER, LOW); // Turn Buzzer Off
    buzzing = false;
  }
}

void setColor(int redValue, int greenValue,  int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin,  greenValue);
  analogWrite(bluePin, blueValue);
}