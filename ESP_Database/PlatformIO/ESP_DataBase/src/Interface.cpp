//---------------------------------------------------------------
// Title: Interface
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: This is a file which combines the previous 
// getTimeAPI.cpp and sendRequest.cpp files into one file which 
// handles the program's connection to the timeapi.io site 
// (and getting the time) as well as connecting to and sending the
// sensor data to the database, in addition to recognizing button presses
// and assigning nodes.
// Inputs: Button (D5), Tilt Ball Switch (D6), KY-015 Temp & Humidity Sensor (D2)
// Outputs: Temperature, Humidity, and Time in JSON format
// Date: 10/18/2025 2:12 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code
//---------------------------------------------------------------------------------
// File Dependencies: WifiCred.h, sendRequest.h, getTimeAPI.h, Interface.h
//---------------------------------------------------------------------------------

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "WifiCred.h"
#include "sendRequest.h"  // Original sendData(), connectionDetails()
#include "getTimeAPI.h"   // Original getTime()
#include "Interface.h"

#include <DHT.h>

#define DHTTYPE DHT11
#define sensorPin D2 // KY-015 Temperature & Humidity Sensor Pin
static DHT dht(sensorPin, DHTTYPE);

volatile bool g_btnEvent  = false;
volatile bool g_tiltEvent = false;

// Interrupts
//---------------------------------------------------------------------------------

IRAM_ATTR void onButtonISR() {
    g_btnEvent = true;
}

IRAM_ATTR void onTiltISR() {
    g_tiltEvent = true;
}

// Constructor Definition
//---------------------------------------------------------------------------------
Interface::Interface() {
    pinMode(btnPin, INPUT); // Button is Active HIGH, uses external Pull-Down
    pinMode(tiltPin, INPUT_PULLUP); // Tilt Switch is Active LOW, uses internal Pull-Up
    
    attachInterrupt(digitalPinToInterrupt(btnPin),  onButtonISR, RISING);  // if pull-down, pressed = HIGH
    attachInterrupt(digitalPinToInterrupt(tiltPin), onTiltISR,  CHANGE);   // KY-020: either edge
    
    dht.begin();
}

// Wifi Status - Initial connection
//---------------------------------------------------------------------------------
void Interface::checkWifi() {
    Serial.println("");
  
    // (A) Start connecting to the WiFI
    Serial.println("\nConnecting to WiFi"); 
    WiFi.begin(ssid, password); // Connection to WiFi Starts until status()=WL_CONNECTED
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print("."); // waiting to get connected
    }
}

// Check which switch has been activated to decide which Node to 
// Send data to.
//---------------------------------------------------------------------------------
void Interface::checkSwitch() {

    static uint32_t lastBtn = 0, lastTilt = 0;
    uint32_t current = millis();

    // If the button has been pressed
    if (g_btnEvent) {
        g_btnEvent = false; // Reset Interrupt flag
        if (current - lastBtn > 50) { // millis() debounce
            node1 = true;
            Serial.println("\nButton pressed. Data will be sent as Node_1.");
            lastBtn = current;
        }
    }

    // If the Tilt Switch has been activated
    if (g_tiltEvent) {
        g_tiltEvent = false; // Reset Interrupt flag
        if (current - lastTilt > 50) { // illis() debounce
            node2 = true;
            Serial.println("\nTilt Switch activated. Data will be sent as Node_2.");
            lastTilt = current;
        }
    }

}

// Get time (uses getTimeAPI.cpp) and convert to SQL format for database
//---------------------------------------------------------------------------------
void Interface::currentTime() {
    String time = ::getTime(); // call global getTime() in getTimeAPI


    if (time.length() > 0) { // If time is not empty, store current time
       
        // convert ISO to MySQL format - NEED TO HAVE THIS TO SEND TO DATABASE!
        // ----------------------------------------------------------------------
        time.replace("T", " ");  // replace 'T' with space
        time.replace("Z", "");   // remove trailing 'Z'

        // URL-encode the space in the query string
        time.replace(" ", "%20");

        // keep ONLY 6 digits after the dot - Prevent 400 Error!
        int dot = time.indexOf('.');
        if (dot > 0 && time.length() > dot + 7) {
            time = time.substring(0, dot + 7); 
        }

        CurrentTime = time;

        Serial.println("\nTime Formatted for DataBase: " + CurrentTime);
    }
}

// Read the Temperature from the KY-015 Sensor
//---------------------------------------------------------------------------------
void Interface::readTemperature() {
    // read DHT; returns NAN on failure
    currentTemp = dht.readTemperature(); // °C
}

// Read the Humidity from the KY-015 Sensor
//---------------------------------------------------------------------------------
void Interface::readHumidity() {
    // read DHT; returns NAN on failure
    currentHum = dht.readHumidity(); // %
}

// Pre-read the Sensor every 2s and store it if valid (not NAN).
// Allows for more immediate response, bc DHT can be slow.
//---------------------------------------------------------------------------------
void Interface::preReadSensor() {
    static uint32_t last = 0;

    if (millis() - last < 2000) return; // Non-blocking millis() delay
    last = millis();

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    // If not NAN, store current values
    if (!isnan(t)) currentTemp = t;
    if (!isnan(h)) currentHum  = h;
}

// Check that sensor data is valid (not NAN)
//---------------------------------------------------------------------------------
void Interface::checkError() {
    if (isnan(currentTemp) || isnan(currentHum)) {
        Serial.println("\nERROR: Sensor Values are Invalid.");
    }
}

// Transmit Data (uses sendRequest.cpp functions)
//---------------------------------------------------------------------------------
void Interface::transmitData() {
    // If data is not being sent to any node, do not send data
    if (!node1 && !node2) return; 

    // Data HAS to include time, or do not send data!
    if (CurrentTime.isEmpty()) {
        Serial.println("ERROR: No time present.");
        node1 = node2 = false;
        return;
    }

    // Check that sensor data is not NAN
    checkError();

    // If either is NAN
    if (isnan(currentTemp) || isnan(currentHum)) {
        Serial.println("ERROR: Invalid sensor values.");
        node1 = node2 = false; // Reset after send attempt
        return;
    }

    // If node 1 = true -> nodeID = node1. Else: node2
    const String nodeID = node1 ? "node_1" : "node_2";

    Serial.println("\nPreparing to Send Data...");

    // Check all data is valid. If yes, print success message.
    // If no, print error message
    bool ok = ::sendData(nodeID, CurrentTime, currentTemp, currentHum); // call global sendData() in sendRequest
    Serial.println(ok ? "\nData Successfully Sent!" : "\nError: Send Failed.");
    node1 = node2 = false; // Reset after send attempt
}

// Only send data when switch = activated
//---------------------------------------------------------------------------------
bool Interface::sendNow(unsigned long interval) {
    static unsigned long lastSend = 0;
    unsigned long now = millis();

    // ONLY Send data when a switch is activated
    // AND use a non-blocking millis() delay as a sort-of debounce
    if ((node1 || node2) &&  (now - lastSend >= interval)) {
        lastSend = now;
        return true; // Data is okay to send
    }
    return false; // Do not send data
}