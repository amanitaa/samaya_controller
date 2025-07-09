#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <core/transmitter.h>

#define CE_PIN A2
#define CSN_PIN A3

RF24 radio(CE_PIN, CSN_PIN);
byte counter = 0;

void setup() {
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println("Radio initialization failed!");
    while (1);
  }
  setupRadio(radio);
  radio.stopListening();
  Serial.println("Transmitter initialized");
}

void loop() {
  Serial.print("Sending... ");
  Serial.println(counter);

  unsigned long last_time = micros();
  byte gotByte;
  if (sendMessage(radio, &counter, 1, &gotByte)) {
    Serial.print("Answer: ");
    Serial.print(gotByte);
    Serial.print(" Time: ");
    Serial.print(micros() - last_time);
    Serial.println(" microseconds");
    counter++;
  } else if (!radio.available()) {
    Serial.print("Empty, Time: ");
    Serial.print(micros() - last_time);
    Serial.println(" microseconds");
  } else {
    Serial.println("Fail");
  }
  Serial.println();

  delay(1000);
}