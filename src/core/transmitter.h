#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <RF24.h>

struct StatusPackage {
    bool isUpsideDown;
    uint8_t liionPercent;
    uint8_t lipoPercent;
};

struct ControlPackage {
  int16_t left;
  int16_t right;
};

void setupRadio(RF24& radio);
bool sendMessage(RF24& radio, const char* data, uint8_t size, StatusPackage* statusResponse);

#endif