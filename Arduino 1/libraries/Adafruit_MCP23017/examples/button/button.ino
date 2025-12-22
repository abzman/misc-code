#include <Wire.h>
#include "Adafruit_MCP23017.h"

/*
 * be sure to use the format: DP=0x01, A=0x40, B=0x80, C=0x08, D=0x04, E=0x02, F=0x20, G=0x10
 * there are no decimal points on the bottom wired up
*/


/* First method to use this code
 * writes contents of bufferRaw to the displays, all four at once
 * 
  writeBufferRaw();
  */
  
/* Second method to use this code
 * writes contents of buffer to the displays, all four at once
 * 
  writeBuffer();
  */
  
/* Third method to use this code
 * writes digit X to display Y, leaving all the others alone
 * 

  writeDigit(digits[X]);
  latchIn(Y);
  */
  
/* Fourth method to use this code
 * writes bits 0xXX to display Y, leaving all the others alone
 * be sure to use the format: DP=0x01, A=0x40, B=0x80, C=0x08, D=0x04, E=0x02, F=0x20, G=0x10
 * there are no decimal points on the bottom wired up

  writeDigit(0xXX);
  latchIn(Y);
  */



 

Adafruit_MCP23017 mcp;

uint8_t displays[] = {0x00, 0x08, 0x04, 0x0C};                                      //this is the lower 4 bits backward, to make more sense it would be wired the other way
uint8_t neutral = 0x01;                                                             //unused display output used as a state to send the latch to when not used
uint8_t digits[] = {0xEE, 0x88, 0xD6, 0xDC, 0xB8, 0x7C, 0x7E, 0xC8, 0xFE, 0xFC};    //hand coded 7-segment font
uint8_t buffer[] = {1, 2, 3, 4};                                                    //buffer that writeBuffer pulls from
uint8_t bufferRaw[] = {0xFA, 0xFE, 0x66, 0xEE};                                     //buffer that writeBufferRaw pulls from

void setup() {
  Serial.begin(115200);
  mcp.begin();                       // use default address 0

  for (int i = 0; i < 12; ++i) {
    mcp.pinMode(i, OUTPUT);
  }


  for (int i = 12; i < 14; ++i) {
    mcp.pinMode(i, INPUT);
    mcp.pullUp(i, HIGH);//100K pull
  }

}

uint16_t writeAddr(uint8_t high) {
  uint16_t a = mcp.readGPIOAB();
  a = high << 8 | (a & 0x00FF);
  mcp.writeGPIOAB(a);
  return a;
}
uint16_t writeDigit(uint8_t low) {
  uint16_t a = mcp.readGPIOAB();
  a = low | (a & 0xFF00);
  mcp.writeGPIOAB(a);
  return a;
}

void latchIn(uint8_t digit) {

  writeAddr(displays[digit]);
  delay(1);
  writeAddr(neutral);
}

void writeBuffer()
{
  for (int i = 0; i < 4; ++i) {
    writeDigit(digits[buffer[i]]);
    latchIn(i);
  }
}

void writeBufferRaw()
{
  for (int i = 0; i < 4; ++i) {
    writeDigit(bufferRaw[i]);
    latchIn(i);
  }
}


void loop() {

  writeBuffer();
  delay(10000);



}
