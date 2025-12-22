// SPDX-FileCopyrightText: 2022 BlueMicro_HID_Arduino_Library contributors (https://github.com/jpconstantineau/BlueMicro_HID_Arduino_Library/graphs/contributors)
//
// SPDX-License-Identifier: BSD-3-Clause

#include <bluemicro_hid.h>
#include "pio_encoder.h"

// This was tested with the following hardware:
// Adafruit nRF52832 Feather - BLE Only
// Adafruit nRF52840 Festher Express - BLE + USB
// Adafruit Feather RP2040 - USB Only
// Adafruit Feather M0 Express - USB Only
// Adafruit Feather M4 Express - USB Only
// The GPIO/Pin differs betwen the feathers.
// No other changes are needed between Feathers.

static bool has_consumer_key2 = false;
static bool has_consumer_key3 = false;
bool lowRes = 0;
const int pin = 3;  // UserSw
bool activeState = true;
const int pin2 = 2;     // UserSw
PioEncoder encoder(5);  // encoder is connected to GPIO2 and GPIO3

// the setup function runs once when you press reset or power the board
void setup() {


  encoder.begin();
  encoder.flip();
  bluemicro_hid.begin();

  // Set up button, pullup opposite to active state
  pinMode(pin, activeState ? INPUT_PULLDOWN : INPUT_PULLUP);
  pinMode(pin2, activeState ? INPUT_PULLDOWN : INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("BlueMicro_HID Composite example");
}

void loop() {
  // poll gpio once each 10 ms
  delay(10);


  bool btn_pressed = (digitalRead(pin) == activeState);
  if (btn_pressed && !has_consumer_key2) {
    bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_MUTE);
    has_consumer_key2 = true;
  } else if (!btn_pressed) {
    if (has_consumer_key2) {
      bluemicro_hid.consumerKeyRelease();
      has_consumer_key2 = false;
    }
  }

  bool btn_pressed2 = (digitalRead(pin2) == activeState);
  int8_t const delta = 5;
  // use to send consumer release report
  static bool has_consumer_key = false;
  int count = encoder.getCount();


  if (btn_pressed2) {
    if (lowRes == 1) {
      encoder.reset();
    }
    lowRes = 0;
    if (count < 0) {
      bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_VOLUME_INCREMENT);
      has_consumer_key = true;
    } else if (count > 0) {
      bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_VOLUME_DECREMENT);
      has_consumer_key = true;
    } else {
      if (has_consumer_key) {
        bluemicro_hid.consumerKeyRelease();
      }
      has_consumer_key = false;
    }
    encoder.reset();
  } else {
    lowRes = 1;
    if (count < -3) {
      bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_BRIGHTNESS_INCREMENT);
      has_consumer_key = true;
      encoder.reset();
    } else if (count > 3) {
      bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_BRIGHTNESS_DECREMENT);
      has_consumer_key = true;
      encoder.reset();
    } else {
      if (has_consumer_key) {
        bluemicro_hid.consumerKeyRelease();
      }
      has_consumer_key = false;
    }
  }
  /*
  if (count > 0) {
    if (btn_pressed2) {
      bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_VOLUME_INCREMENT);
      has_consumer_key = true;
    } else {
      bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_BRIGHTNESS_INCREMENT);
      has_consumer_key = true;
    }
    encoder.reset();
  } else if (count < 0) {
    if (btn_pressed2) {
      bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_VOLUME_DECREMENT);
      has_consumer_key = true;
    } else {
      bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_BRIGHTNESS_DECREMENT);
      has_consumer_key = true;
    }
    encoder.reset();
  } else {
    if (has_consumer_key) {
      bluemicro_hid.consumerKeyRelease();
    }
    has_consumer_key = false;
  }
  */
  bluemicro_hid.processQueues(CONNECTION_MODE_AUTO);
}
