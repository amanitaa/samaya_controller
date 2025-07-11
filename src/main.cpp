#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "core/transmitter.h"
#include "ui/display.h"

#define CE_PIN A2
#define CSN_PIN A3

#define JOY_X_PIN A0
#define JOY_Y_PIN A1

#define DEADZONE 0.1
#define MAX_SPEED 255
#define SEND_INTERVAL_MS 300

RF24 radio(CE_PIN, CSN_PIN);

struct ControlPackage {
  int16_t left;
  int16_t right;
};

void getMotorSpeeds(float x, float y, int16_t& left_speed, int16_t& right_speed) {
  if (abs(x) < DEADZONE) x = 0;
  if (abs(y) < DEADZONE) y = 0;

  y = -y;

  float left = y + x;
  float right = y - x;

  left = max(-1.0f, min(1.0f, left));
  right = max(-1.0f, min(1.0f, right));

  left_speed = (int16_t)round(left * MAX_SPEED);
  right_speed = (int16_t)round(right * MAX_SPEED);
}

void setup() {
  Serial.begin(9600);

  pinMode(JOY_X_PIN, INPUT);
  pinMode(JOY_Y_PIN, INPUT);

  setupDisplay();
  updateDisplay(0, 0, RF_CHANNEL, false);

  if (!radio.begin()) {
    Serial.println("Radio initialization failed!");
    updateDisplayMessage("Radio Init Failed");
  } else {
    setupRadio(radio);
    radio.stopListening();
    Serial.println("Transmitter initialized");
  }
}

void loop() {
  static ControlPackage lastSent = {0, 0};
  static unsigned long lastSentTime = 0;
  static bool lastUpsideDown = false;
  static uint8_t lastChannel = RF_CHANNEL;

  float x = (analogRead(JOY_X_PIN) - 512.0) / 512.0;
  float y = (analogRead(JOY_Y_PIN) - 512.0) / 512.0;

  int16_t left_speed, right_speed;
  getMotorSpeeds(x, y, left_speed, right_speed);

  ControlPackage command = {left_speed, right_speed};

  updateDisplay(command.left, command.right, lastChannel, lastUpsideDown);

  unsigned long now = millis();
  if (radio.isChipConnected() &&
      (command.left != lastSent.left || command.right != lastSent.right || now - lastSentTime > SEND_INTERVAL_MS)) {
    Serial.print("Sending: L=");
    Serial.print(command.left);
    Serial.print(", R=");
    Serial.println(command.right);

    unsigned long last_time = micros();
    StatusPackage status = {false};
    if (sendMessage(radio, &command, sizeof(ControlPackage), &status)) {
      lastUpsideDown = status.isUpsideDown;
      lastChannel = radio.getChannel();
      Serial.print(" Time: ");
      Serial.print(micros() - last_time);
      Serial.println(" microseconds");
      lastSent = command;
      lastSentTime = now;
    } else if (!radio.available()) {
      Serial.print("Empty, Time: ");
      Serial.print(micros() - last_time);
      Serial.println(" microseconds");
    } else {
      Serial.println("Fail");
    }
    Serial.println();
  }

  delay(20);
}