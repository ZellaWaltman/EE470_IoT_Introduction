//---------------------------------------------------------------
// Title: main
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: This is the main program for the ESP8266 programming
// assignment. Temperature and Humidity data is collected when a button or
// tilt switch is activated. The switch type determines whether data will
// be sent to node_1 (button) or node_2 (tilt switch). The program will 
// connect the ESP8266 to wifi, at which point the time will be extracted
// from the timeapi.io site, and the data will then be sent to the database.
// Inputs: Button (D5), Tilt Ball Switch (D6), KY-015 Temp & Humidity Sensor (D2)
// Outputs: Temperature, Humidity, and Time in JSON format
// Date: 10/18/2025 2:12 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code
//---------------------------------------------------------------
// File Dependencies: 
//---------------------------------------------------------------


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "sendRequest.h"  // Original sendData(), connectionDetails()
#include "getTimeAPI.h"   // Original getTime()
#include "Interface.h"

Interface connect; 

void setup () {
  Serial.begin(9600);
  delay(10); // just a short delay
  connect.checkWifi(); // Check Wifi Connection
  connectionDetails(); // Get Connection Info
}

void loop() {

    // Check switch state constantly for quick feedback to user
    connect.checkSwitch();

    // Pre-read sensors (make up for slow DHT timing)
    connect.preReadSensor();

    // if 1.5s (1500 ms) debounce has passed & actuator pressed
    if (connect.sendNow(1500)) {
      connect.currentTime();     // get and format the time
      connect.transmitData();    // send data to DB if triggered
    }

    yield();  // Quickly pause and let WiFi tasks run in background

}