#include <Arduino.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <SPI.h>
#include "transmitter.h"

byte controller_node[6] = "NodeA";
byte samaya_node[6] = "NodeB";

#define RF_CHANNEL 0x60
#define RF_DATA_RATE RF24_250KBPS
#define RF_AUTO_ACK true
#define RF_RETRIES_DELAY 0
#define RF_RETRIES_COUNT 15
#define RF_PAYLOAD_SIZE 32
#define RF_ACK_PAYLOAD true


void setupRadio(RF24& radio) {
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(samaya_node);
  radio.openReadingPipe(1, controller_node);
  radio.startListening();
  size_t maxPayloadSize = max(sizeof(ControlPackage), sizeof(StatusPackage));
  radio.setPayloadSize(maxPayloadSize);
}

bool sendMessage(RF24& radio, const void* data, uint8_t size, StatusPackage* statusResponse) {
  radio.stopListening();

  unsigned long start_time = micros();      

  if (!radio.write(data, sizeof(size))) {
    Serial.println(F("failed"));
  }

  radio.startListening();

  unsigned long started_waiting_at = micros();
  boolean timeout = false; 

  while (!radio.available()) {                            
    if (micros() - started_waiting_at > 200000) {
      timeout = true;
      break;
    }
  }

  if (timeout) {
    Serial.println(F("Failed, response timed out."));
    return false;
  } else {
    radio.read(statusResponse, sizeof(StatusPackage));
    unsigned long end_time = micros();
    return true;
  }
}
