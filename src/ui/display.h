#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1

extern Adafruit_SSD1306 display;

void setupDisplay();
void updateDisplay(int16_t leftSpeed, int16_t rightSpeed, uint8_t channel, bool isUpsideDown);
void updateDisplayMessage(const char* message);

#endif
