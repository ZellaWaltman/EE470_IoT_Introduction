// -----------------------------
// Title: Random Number Plotter
// -----------------------------
// Program Detail: 
// -----------------------------
// Purpose: This program generates a random number (0 to 100) every 5
// seconds, stores the values in an array, and prints these values to
// the terminal. These values are then plotted using the serial plotter.

// Inputs: This program uses the random() function to generate random numbers.
// Outputs: The outputs of this program are a random number (0-100) every 5 seconds.
// Date: 09/19/2025 12:04 PM PT
// Compiler: Arduino IDE 2.3.6
// Author: Zella Waltman
// Versions:
//          V1.0 = Original Code

// -----------------------------
// File Dependencies: None
// -----------------------------

// -----------------------------
// Main Program
// -----------------------------

#include <Arduino.h>

#define LED 12

int INTERVAL = 1000; // # of ms for delay
bool STATE = false; // LED state (ON or OFF)

void setup() {
  Serial.begin(9600); // Baud Rate
  pinMode(LED, OUTPUT); // Pin 12 (D6) is output
}

void loop() {
  if (Serial.available() > 0) { // If user types in console
    char entry = (char)Serial.read(); // Get char from user input (A or B)
    Serial.println(entry); // Print what user typed in console

    if (entry == 'A') { // Faster interval if user types 'A'
      INTERVAL = 200;
    }
    else if (entry == 'B') { // Slower interval if user types 'B'
      INTERVAL = 2500;
    }
    else { // Normal interval (Pre-Input)
      INTERVAL = 1000;
    }
  }

    digitalWrite(LED, !digitalRead(LED)); // Flip LED On or Off
    STATE = !STATE; // Says if LED is on or off

    if (STATE == true) { // If state = true, LED ON
      Serial.println("LED is ON");
    }
    else { // If state = false, LED OFF
      Serial.println("LED is OFF");
    }

    delay(INTERVAL); // Delay for which light should change

}
