#include <Arduino.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include "transmitter.h"

const byte slaveAddress[5] = {'R','x','A','A','A'};

#define RF_CHANNEL 0x60
#define RF_PA_LEVEL RF24_PA_MAX
#define RF_DATA_RATE RF24_250KBPS
#define RF_AUTO_ACK true
#define RF_RETRIES_DELAY 0
#define RF_RETRIES_COUNT 15
#define RF_PAYLOAD_SIZE 32
#define RF_ACK_PAYLOAD true


void setupRadio(RF24& radio) {
  // radio.setPALevel(RF_PA_LEVEL);
  radio.setDataRate(RF_DATA_RATE);
  // radio.setChannel(RF_CHANNEL);
  // radio.setAutoAck(RF_AUTO_ACK);
  radio.setRetries(RF_RETRIES_DELAY, RF_RETRIES_COUNT);
  // radio.setPayloadSize(RF_PAYLOAD_SIZE);
  // if (RF_ACK_PAYLOAD) {
    // radio.enableAckPayload();
  // }
  radio.openWritingPipe(slaveAddress);
  // radio.openReadingPipe(1, RX_ADDRESS);
  // radio.powerUp();
}

bool sendMessage(RF24& radio, const void* data, uint8_t size, StatusPackage* statusResponse) {
  bool success = radio.write(data, size);
  if (success && radio.available()) {
    if (radio.isAckPayloadAvailable()) {
      radio.read(statusResponse, sizeof(StatusPackage));
      return true;
    }
    Serial.println("  Acknowledge but no data ");
  }
  return false;
}

bool receiveMessage(RF24& radio, void* data, uint8_t size) {
  if (radio.available()) {
    radio.read(data, size);
    return true;
  }
  return false;
}