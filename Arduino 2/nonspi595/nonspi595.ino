// Count 0-255 74HC595

#define DATA 11 // 74HC595 pin 14
#define LATCH 10 // 74HC595 pin 12
#define CLK 13 // 74HC595 pin 11


void setup()   {
  pinMode(DATA, OUTPUT);  
  pinMode(LATCH, OUTPUT);
  pinMode(CLK, OUTPUT);
  digitalWrite(DATA, 0); 
  digitalWrite(LATCH, 0);
  digitalWrite(CLK, 0);
} // end setup

void loop()   {
  for(int i=0; i<=255; i++)   {
    byteOut(i);
    delay(200);
  }

} //end loop

// use just k instead of !k 
// for common cathode displays.
void   byteOut(byte j)   {
  byte k;
  for(int i=0; i < 8; i++)   {
    k = j & 0x01;
    digitalWrite(DATA, k); 
    pulseCLK();
    j = j >> 1;
  }
  pulseCS(); // latch byte
}


void pulseCLK()   {
  digitalWrite(CLK, HIGH);
  digitalWrite(CLK, LOW); 
}

void pulseCS()   {
  digitalWrite(LATCH, HIGH);
  digitalWrite(LATCH, LOW);
}
