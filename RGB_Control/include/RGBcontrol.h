// 11/01/2025 10:47 PM

#ifndef RGBCONTROL_H
#define RGBCONTROL_H

#include <Arduino.h>

static bool parseCSV_RGB(const String& s, uint8_t &r, uint8_t &g, uint8_t &b);
void RGB_Initialize();
void setColor(int redValue, int greenValue,  int blueValue); 
bool RGB_UpdateFromURL(); 

#endif // RGBCONTROL_H