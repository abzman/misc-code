#include "SPI.h"

const int ss = 10;
SPISettings set595(200000, LSBFIRST, SPI_MODE0); 

byte i = 0;
void setup() {
  // put your setup code here, to run once:
pinMode (ss, OUTPUT);

  digitalWrite (ss, LOW);
  SPI.begin();
  //SPI.setBitOrder(MSBFIRST);
  Serial.begin(115200);
}

void loop() {
  SPI.beginTransaction(set595);
  digitalWrite (ss, LOW);
  SPI.transfer(i);
  digitalWrite (ss, HIGH);
  i = SPI.transfer(0);
  SPI.endTransaction();
  //i += 1;
  delay(10);
}
