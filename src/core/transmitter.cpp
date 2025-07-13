#include <Arduino.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include "transmitter.h"

byte controller_node[6] = "0768";
byte samaya_node[6] = "3163";

const uint8_t channel = 88;

#define RF_DATA_RATE RF24_250KBPS
#define RF_AUTO_ACK true
#define RF_RETRIES_DELAY 0
#define RF_RETRIES_COUNT 15
#define RF_PAYLOAD_SIZE 32
#define RF_ACK_PAYLOAD true


void setupRadio(RF24& radio) {
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(channel);
  radio.openWritingPipe(samaya_node);
  radio.openReadingPipe(1, controller_node);
  radio.startListening();
  radio.setPayloadSize(32);
}

bool sendMessage(RF24& radio, const char* data, uint8_t size, StatusPackage* statusResponse) {

}
