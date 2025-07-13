#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include "core/transmitter.h"
#include "ui/display.h"

#define CE_PIN A2
#define CSN_PIN A3

#define JOY_X_PIN A0
#define JOY_Y_PIN A1

#define DEADZONE 0.2
#define MAX_SPEED 255
#define SEND_INTERVAL_MS 300

int16_t lastSentLeft = 0;
int16_t lastSentRight = 0;

RF24 radio(CE_PIN, CSN_PIN);


void getMotorSpeeds(float x, float y, int16_t& left_speed, int16_t& right_speed) {
  if (abs(x) < DEADZONE) x = 0;
  if (abs(y) < DEADZONE) y = 0;

  x = -x;

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
  updateDisplay(0, 0, 88, false, 0, 0);

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
  static unsigned long lastSentTime = 0;

  float x = (analogRead(JOY_X_PIN) - 512.0) / 512.0;
  float y = (analogRead(JOY_Y_PIN) - 512.0) / 512.0;

  int16_t currentLeftSpeed, currentRightSpeed;
  getMotorSpeeds(x, y, currentLeftSpeed, currentRightSpeed);

  StatusPackage status;
  unsigned long now = millis();
  if (currentLeftSpeed != lastSentLeft || currentRightSpeed != lastSentRight || now - lastSentTime > SEND_INTERVAL_MS) {
    char dataString[16];
    snprintf(dataString, sizeof(dataString), "L%dR%d", currentLeftSpeed, currentRightSpeed);

    radio.stopListening();

    if (!radio.write(&dataString, sizeof(dataString))) {
      Serial.println(F("Failed"));
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
    } else {
      radio.read(&status, sizeof(StatusPackage));
      lastSentLeft = currentLeftSpeed;
      lastSentRight = currentRightSpeed;
      lastSentTime = now;
      updateDisplay(
        currentLeftSpeed,
        currentRightSpeed,
        radio.getChannel(),
        status.isUpsideDown,
        status.liionPercent,
        status.lipoPercent
      );
    }
  }
  delay(20);
}