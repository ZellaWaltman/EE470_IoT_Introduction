#include "WifiCred.h"

// Constants
// ------------------------------
const char* ssid     = "BigPapa"; // must be exactly as it apears - case sensitive
const char* password = "wificonnect";

// Wifi Status - Initial Connection
//---------------------------------------------------------------------------------
void checkWifi() {
    Serial.println("");
  
    // (A) Start connecting to the WiFI
    Serial.println("\nConnecting to WiFi"); 
    WiFi.begin(ssid, password); // Connection to WiFi Starts until status()=WL_CONNECTED
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print("."); // waiting to get connected
    }
}

// Wifi Status - Get Connection Details
//---------------------------------------------------------------------------------
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