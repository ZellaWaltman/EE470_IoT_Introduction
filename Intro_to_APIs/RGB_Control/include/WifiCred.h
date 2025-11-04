//---------------------------------------------------------------
// Title: WifiCred.h
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: This is a header file which stores the ssid and password
// for the wifi.
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

#ifndef WIFICRED_H 
#define WIFICRED_H 

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Declare (not define) these here
extern const char* ssid;
extern const char* password;

// Function Declarations
// ------------------------------
void checkWifi();
void connectionDetails();

#endif // WIFICRED_H 