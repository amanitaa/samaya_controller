#ifndef TRANSMITTER_H
#define TRANSMITTER_H

#include <RF24.h>

void setupRadio(RF24& radio);
bool sendMessage(RF24& radio, const void* data, uint8_t size, byte* response);
bool receiveMessage(RF24& radio, void* data, uint8_t size);

#endif