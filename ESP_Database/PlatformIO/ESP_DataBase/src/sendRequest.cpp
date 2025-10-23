//-----------------------------
// Title: sendRequest
//-----------------------------
// Program Details:
//-----------------------------
// Purpose: Connect to the database site and insert data
// Inputs: URL + Parameters
// Outputs: GET response
// Dependencies: sendRequest.h 
// Compiler: PIO Version 3.3.4
// Author: Zella Waltman
// Versions: 
//  v1: Oct-24-2022 - Original Program
//  v2: Oct-22-2025 - Newest Version with Edits from Zella
//-----------------------------

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "sendRequest.h" // required header file
//#include <WiFiClientSecure.h> // not needed

static const char* BASE_URL = "https://zellawaltman.org/ESP_Program.php";

void connectionDetails()
{
  // Details of the connection
  Serial.println("---------------------");    
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
  Serial.print("Channel:\t");
  Serial.println(WiFi.channel());          
  Serial.print("RSSI:\t");
  Serial.println(WiFi.RSSI());  
  Serial.print("MAC Address:\t");
  Serial.println(WiFi.macAddress()); 
  Serial.print("DNS IP:\t");
  Serial.println(WiFi.dnsIP(1));            // get the DNS IP Address    
  Serial.print("Gateway IP:\t");
  Serial.println(WiFi.gatewayIP()); 
  Serial.println("---------------------\n");    
}

bool sendData(const String& nodeId, const String& timeReceived, float nodeTemp, float humidity) {
  String myReq = ""; // Default Return for Failure
  
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient https;

    String fullUrl = String(BASE_URL) +
                    "?node_name=" + nodeId +
                    "&time_received=" + timeReceived +
                    "&temperature=" + String(nodeTemp,3) +
                    "&humidity=" + humidity;

    Serial.println("Requesting: --> " + fullUrl);
    
    if (https.begin(client, fullUrl)) { // start the connection 1=started / 0=failed

      int httpCode = https.GET(); // choose GET or POST method
      //int httpCode = https.POST(fullUrl); // need to include URL
          
      Serial.println("Response code <--: " + String(httpCode)); // print response code: e.g.,:200

      if (httpCode > 0) {
          //Serial.println(https.getString()); // this is the content of the get request received
      }

        https.end(); // end of request
        return true;

    } else {

        Serial.printf("[HTTPS] Unable to connect\n");
        return false;

    }
  }

  delay(5000); // delay between each REQUEST to the server
  return false; // Default return

}