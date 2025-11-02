#include <Arduino.h>
#include "WifiCred.h"
#include "button.h"
#include "PHPcontrol.h"

void setup() {

  Serial.begin(9600);

  buttonInit();
  LEDInit();

  checkWifi(); // Check Wifi Connection
  connectionDetails(); // Get Connection Info

  // Do one fetch at boot
  fetchAndApply();
}

void loop() {
  buttonPress(); // Check if Button is pressed

  // Short sleep to reduce CPU churn
  delay(10);
}