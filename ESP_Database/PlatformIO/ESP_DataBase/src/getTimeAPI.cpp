
//-----------------------------
// Title: GetTimeAPI
//-----------------------------
// Program Details:
//-----------------------------
// Purpose: Connect to the timeapi.io site and GET time
// Inputs: None
// Outputs: Date and Time 
// Dependencies: No external file is required
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions: 
//  v1: Oct-24-2022 - Original Program
//  v2: Oct-22-2025 - Newest Version with Edits from Zella
//-----------------------------

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>  // must be installed 
#include "getTimeAPI.h"

String url = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";

DynamicJsonDocument doc(1024);

String getTime() {

  String mytime = ""; // Default Return for Failure

  // Make sure Wi-Fi is connected
  if (WiFi.status() == WL_CONNECTED) {

    WiFiClientSecure client;
    client.setInsecure(); // Skip cetrificate validation

    HTTPClient https;
    https.setTimeout(7000); // Wait for the server to respond for 7 seconds before giving up

    String fullUrl = url; // preparing the full URL
    Serial.println("\nRequesting: --> " + fullUrl);

    if (https.begin(client, fullUrl)) { // start the connection 1=started / 0=failed

      int httpCode = https.GET(); // choose GET or POST method
      //int httpCode = https.POST(fullUrl); // need to include URL
      
      Serial.println("\nResponse code <--: " + String(httpCode)); // print response code: e.g.,:200
      
      // If we get a time, parse it
      if (httpCode > 0) {
        String payload = https.getString();
        Serial.println(payload); // this is the content of the get request received
        
        // Parse ONCE
        DeserializationError err = deserializeJson(doc, payload);
        if (!err) {
          mytime = doc["dateTime"].as<String>();  // e.g. "2025-10-18T22:31:44.1790514"
          Serial.println("ISO from API: " + mytime);
        } else { // If there is an error
          Serial.print("JSON parse error: ");
          Serial.println(err.c_str());
        }
      }

      https.end();

    } else { // Error Occurs
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  return mytime; // Default Return

}