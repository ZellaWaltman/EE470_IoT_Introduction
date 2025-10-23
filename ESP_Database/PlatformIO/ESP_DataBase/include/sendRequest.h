//---------------------------------------------------------------
// Title: sendRequest.h
//---------------------------------------------------------------
// Program Detail:
//---------------------------------------------------------------
// Purpose: This is a header file for the sendRequest.cpp file, which 
// connects to the timeapi.io site and GETs the time.
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

#ifndef SENDREQUEST_H 
#define SENDREQUEST_H

#include <Arduino.h>

void connectionDetails();

bool sendData(const String& nodeId, const String& timeReceived, float nodeTemp, float humidity);

#endif // SENDREQUEST_H