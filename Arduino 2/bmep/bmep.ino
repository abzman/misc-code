
#include <U8g2lib.h>
#include <stdint.h>
#include "Wire.h"

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 5, /* data=*/ 4);

char buf[512];

uint8_t readRegister(uint8_t offset)
{
  //Return value
  uint8_t result = 0;

  Wire.beginTransmission(0x76);
  Wire.write(offset);
  Wire.endTransmission();

  Wire.requestFrom(0x76, 1);
  while ( Wire.available() ) // slave may send less than requested
  {
    result = Wire.read(); // receive a byte as a proper uint8_t
  }


  return result;
}

uint8_t detector()
{

  byte error, address;
  int nDevices;

  nDevices = 0;
  for (address = 116; address < 119; address++ ) //does not detect the screen, only the bmp/bme
  {

    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      nDevices++;
    }
  }
  if (nDevices == 0)
  {
    return 0;
  }
  else
  {

    uint8_t chipID = readRegister(0xD0); //Should return 0x60 or 0x58

    switch (chipID) {
      case 0x00:
        return 0;
        break;
      case 0x58:
        return 1;
        break;
      case 0x60:
        return 2;
        break;
      default:
        return 3;
        break;
    }
  }

}

void setup()
{
  // --- Display ---
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14_mf);
  u8g2.drawStr(0, 10, "BME/BMP Detector");
  u8g2.drawStr(30, 30, "v0.69.1");
  u8g2.sendBuffer();
  delay(2000);

}

void loop() {
  u8g2.clearBuffer();
  u8g2.drawStr(0, 10, "BME/BMP Detector");

  uint8_t chip = detector();

  switch (chip) {
    case 0:
      sprintf(buf, "sensor: NONE");
      break;
    case 1:
      sprintf(buf, "sensor: BMP");
      break;
    case 2:
      sprintf(buf, "sensor: BME");
      break;
    case 3:
      sprintf(buf, "sensor: UNKNOWN");
      break;
    default:
      sprintf(buf, "sensor: UNKNOWN");
      break;
  }
  u8g2.drawStr(0, 50, buf);
  u8g2.sendBuffer();
  delay(1000);
}
