//---------------------------------------------------------------
// Title: Interface.h
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: This is a header file which combines the previous 
// getTimeAPI.h and sendRequest.h files into one header file. which 
// handles the program's connection to the timeapi.io site 
// (and getting the time)
// Inputs: none
// Outputs: none
// Date: 10/18/2025 2:12 PM PT
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions:
//      V1.0 Original Code
//---------------------------------------------------------------
// File Dependencies: none
//---------------------------------------------------------------

#ifndef INTERFACE_H 
#define INTERFACE_H

#include <Arduino.h>

class Interface {
    public:
        Interface();
        
        void checkWifi();
        void checkSwitch();
        void currentTime();
        void preReadSensor();
        void readTemperature();
        void readHumidity();
        void checkError();
        void transmitData();
        bool sendNow(unsigned long interval);

    private:

        // Actuator Pins
        const uint8_t btnPin  = D5; // Button (Node 1)
        const uint8_t tiltPin = D6; // KY-020 Tilt Switch Sensor (Node 2)

        // State
        String CurrentTime;
        float  currentTemp = NAN;
        float  currentHum  = NAN;

        // Active Node Flags
        bool node1 = false;
        bool node2 = false;
};

#endif // INTERFACE_H 