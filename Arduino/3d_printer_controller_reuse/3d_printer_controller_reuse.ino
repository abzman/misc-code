/*
Monoprice MP Select Mini v1 lcd control board and rotary encoder

rebranded Malyan M200 - V1

screen is Huawei U8510 IDEOS X3 (replacement display without touch)
    TM032PDZ04 
Himax HX8357
 
 
 pinout:
 gpio0, pushbutton on encoder, active low
 gpio2, pulled up all the time?
 gpio4, one of the encoder lines
 gpio5, the other encoder line
 the led drive signal comes off the lcd header
 GPIO14 and it is used in SPI as clock pin (SPI_CLK)
 GPIO12 power supply?
 GPIO13 and it is used in SPI as Master-Out Slave-In pin (SPI_MOSI)
 GPIO15 and it is used in SPI as Chip Select pin (SPI_CS)
*/



#include <Arduino.h>

#include "HX8357.h"

extern "C" {
#include "sdk_spi.h"
#include "spi_interface.h"
}

HX8357 display = HX8357();

#include <ESP8266WiFi.h>
#include <RotaryEncoder.h>
#include <Ticker.h>

#define PIN_A 4   //ky-040 clk pin, add 100nF/0.1uF capacitors between pin & ground!!!
#define PIN_B 5   //ky-040 dt  pin, add 100nF/0.1uF capacitors between pin & ground!!!
#define BUTTON 0  //ky-040 sw  pin, add 100nF/0.1uF capacitors between pin & ground!!!

int16_t position = 0;

RotaryEncoder encoder(PIN_A, PIN_B, BUTTON);

Ticker encoderRotary;
Ticker encoderButton;


void encoderISR() {
  encoder.readAB();
}

void encoderButtonISR() {
  encoder.readPushButton();
}
void setup() {
  //encoder.begin();  //set encoders pins as input & enable built-in pullup resistors

  //encoderRotary.attach_ms(10, encoderISR);        //call encoderISR()       every 10 milliseconds/0.010 seconds
  //encoderButton.attach_ms(15, encoderButtonISR);  //call encoderButtonISR() every 15 milliseconds/0.015 seconds
  Serial.begin(115200);
  while (!Serial) {};

  Serial.println("Initializing Display...");

  display.begin();

  display.setRotation(LANDSCAPE_RIGHT);

  display.setCursor(10, 10);
  display.setTextSize(10);
  display.setTextColor(0x4FE9);
  display.print("Hacked!");

  display.setCursor(10, 100);
  display.setTextSize(10);
  display.setTextColor(0x4FE9);
  display.print("location");
}

void loop() {
  /*
    if (position != encoder.getPosition())
  {
    position = encoder.getPosition();
    Serial.println(position);
  }
  
  if (encoder.getPushButton() == true) Serial.println(F("PRESSED")); //(F()) save string to flash & keep dynamic memory free
*/
}
