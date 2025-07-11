#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <RF24.h>

#define RF_CHANNEL 0x60

struct StatusPackage {
  bool isUpsideDown;
};

void setupRadio(RF24& radio);
bool sendMessage(RF24& radio, const void* data, uint8_t size, StatusPackage* statusResponse);
bool receiveMessage(RF24& radio, void* data, uint8_t size);

#endif