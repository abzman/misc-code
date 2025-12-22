#include <EEPROM.h>

void setup()
{
  Serial.begin(115200);

  uint addr = 0;

  // fake data
  struct { 
    uint8_t magic = 0;
    uint8_t mode = 0;
    uint8_t swing = 0;
    uint8_t timeout = 0;
  } data;

  // commit 512 bytes of ESP8266 flash (for "EEPROM" emulation)
  // this step actually loads the content (512 bytes) of flash into 
  // a 512-byte-array cache in RAM
  EEPROM.begin(8);

  // read bytes (i.e. sizeof(data) from "EEPROM"),
  // in reality, reads from byte-array cache
  // cast bytes into structure called data
  EEPROM.get(addr,data);
  Serial.println();
  Serial.println("Old values are: "+String(data.magic)+","+String(data.mode)+","+String(data.swing)+","+String(data.timeout));

  if (data.magic == 49)
  {
  Serial.println("valid data");
  
  data.mode += 1;
  data.swing += 2;
  data.timeout += 3;
  }
  else 
  {
  Serial.println("invalid data, fixing");
  data.magic = 49;
  data.mode = 1;
  data.swing = 2;
  data.timeout = 3;
  }
  if (data.timeout >= 50)
  {
    data.magic = 17;
  Serial.println("intentionally corrupted data to prove repair procedure");
  }
  // replace values in byte-array cache with modified data
  // no changes made to flash, all in local byte-array cache
  EEPROM.put(addr,data);

  // actually write the content of byte-array cache to
  // hardware flash.  flash write occurs if and only if one or more byte
  // in byte-array cache has been changed, but if so, ALL 512 bytes are 
  // written to flash
  EEPROM.commit();  

  // clear 'data' structure
  data.magic = 0x00;
  data.mode = 0x00;
  data.swing = 0x00;
  data.timeout = 0x00;

  // reload data for EEPROM, see the change
  //   OOPS, not actually reading flash, but reading byte-array cache (in RAM), 
  //   power cycle ESP8266 to really see the flash/"EEPROM" updated
  EEPROM.get(addr,data);
  Serial.println("New values are: "+String(data.magic)+","+String(data.mode)+","+String(data.swing)+","+String(data.timeout));

}

void loop()
{
  delay(1000);
}
