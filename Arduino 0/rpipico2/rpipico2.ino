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
const int pin = 2;  // UserSw
bool activeState = false;
const int pin2 = 3;     // UserSw
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

  // use to send consumer release report
  static bool has_consumer_key = false;
  int count = encoder.getCount();
  if (count > 0) {
    bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_VOLUME_INCREMENT);
    has_consumer_key = true;
    encoder.reset();
  } else if (count < 0) {
    bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_VOLUME_DECREMENT);
    has_consumer_key = true;
    encoder.reset();
  } else {
    if (has_consumer_key) {
      bluemicro_hid.consumerKeyRelease();
    }
    has_consumer_key = false;
  }

  /*------------- Keyboard -------------*/
  // Consumer Control is used to control Media playback, Volume, Brightness etc ...
  // Consumer report is 2-byte containing the control code of the key
  // For list of control check out https://github.com/hathach/tinyusb/blob/master/src/class/hid/hid.h

  // use to send consumer release report


  bool btn_pressed = (digitalRead(pin) == activeState);
  if (btn_pressed && !has_consumer_key2) {
      uint8_t keycode[6] = { 0 };
      keycode[0] = HID_KEY_SPACE;
      bluemicro_hid.keyboardReport(0, keycode);
    has_consumer_key2 = true;
  } else if (!btn_pressed) {
    if (has_consumer_key2) {
      bluemicro_hid.keyboardRelease();
      has_consumer_key2 = false;
    }
  }
  bool btn_pressed2 = (digitalRead(pin2) == activeState);
  if (btn_pressed2 && !has_consumer_key3) {
    // send volume down (0x00EA)
    bluemicro_hid.consumerKeyPress(HID_USAGE_CONSUMER_MUTE);
    has_consumer_key3 = true;
  } else if (!btn_pressed2) {
    if (has_consumer_key3) {
      bluemicro_hid.consumerKeyRelease();
      has_consumer_key3 = false;
    }
  }
  bluemicro_hid.processQueues(CONNECTION_MODE_AUTO);
}
