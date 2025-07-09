#include <Arduino.h>
#include <RF24.h>
#include "config.h"

void setupRadio(RF24& radio) {
  radio.setPALevel(RF_PA_LEVEL);
  radio.setDataRate(RF_DATA_RATE);
  radio.setChannel(RF_CHANNEL);
  radio.setAutoAck(RF_AUTO_ACK);
  radio.setRetries(RF_RETRIES_DELAY, RF_RETRIES_COUNT);
  radio.setPayloadSize(RF_PAYLOAD_SIZE);
  if (RF_ACK_PAYLOAD) {
    radio.enableAckPayload();
  }
  radio.openWritingPipe(TX_ADDRESS); // Send to receiver
  radio.openReadingPipe(1, RX_ADDRESS); // Receive from receiver
  radio.powerUp();
}

bool sendMessage(RF24& radio, const void* data, uint8_t size, byte* response) {
  radio.stopListening();
  bool success = radio.write(data, size);
  if (success && radio.available()) {
    radio.read(response, 1);
    return true;
  }
  return false;
}