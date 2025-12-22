#include <Wire.h>

int BH1750_address = 0x23; // i2c Addresse
byte buff[2];

void setup(){
  
  Serial.begin(115200);
  Serial.println("Starte Beleuchtungsstaerkemessung - blog.simtronyx.de");
  
  Wire.begin();
  
  Serial.println("Starte Beleuchtungsstaerkemessung - blog.simtronyx.de");
  BH1750_Init(BH1750_address);
  
  Serial.println("Starte Beleuchtungsstaerkemessung - blog.simtronyx.de");
  
  delay(200);
}

void loop(){
  
  float valf=0;

  if(BH1750_Read(BH1750_address)==2){
    
    valf=((buff[0]<<8)|buff[1])/1.2;
    
    if(valf<0)Serial.print("> 65535");
    else Serial.print((int)valf,DEC); 
    
    Serial.println(" lx"); 
  }
  delay(1000);
}

void BH1750_Init(int address){
  
  Wire.beginTransmission(address);
  
  Serial.println("init");
  Wire.write(0x10); // 1 [lux] aufloesung
  Serial.println("init");
  //Wire.endTransmission();
  Serial.println("init");
}

byte BH1750_Read(int address){
  
  byte i=0;
  //Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()){
    buff[i] = Wire.read(); 
    i++;
  }
  //Wire.endTransmission();  
  return i;
}


