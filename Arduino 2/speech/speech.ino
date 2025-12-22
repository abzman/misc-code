
// Pin definitions
// Note: pin 2 is the high data bit
int data[] = {
  2,3,4,5,6,7,8,9};  // D0-D7, MSB-LSB
int a = 10;
int b = 11;
int g1 = 12;  // active high
int led = 13;
int gb2 = A0; // active low
int rdy = A1; // active high when ready (needs pullup tho)

// Commands, high bit first
const uint8_t CMD_RESET = 0x70;
const uint8_t CMD_LOADADDR = 0x40;
const uint8_t CMD_SPEAK = 0x50;
const uint8_t CMD_SPEAKEXT = 0x60;
const uint8_t CMD_READBYTE = 0x10;
const uint8_t CMD_READANDBRANCH = 0x30;

// This is the word "GOOD"
const uint8_t SPEECH_GOOD_LEN = 0x24;
const uint8_t SPEECH_GOOD[SPEECH_GOOD_LEN] = {
                                0xc5,0xb1, 0x94,0x1a,0x3c,0xcc,0xfb,0x37,0x28,0x9a,
  0x16,0xd6,0x52,0x6f,0xda,0xbb,0x57,0x6a, 0xe5,0x4d,0x63,0xd4,0xbc,0x5c,0x37,0x5d,
  0xeb,0x55,0x64,0x3d,0x4b,0xc4,0xb8,0x3c, 0x00,0x00
};

// This is the address of the word "GOOD" in the speech roms
const uint32_t ADDRESS_GOOD = 0x30d6;

void setup() {      
  // Databus to input
  for (int i = 0;i<8;i++)
  {
    pinMode(data[i], INPUT);
  }
  
  digitalWrite(g1, LOW);  // deassert read and write
  pinMode(g1, OUTPUT);       
  pinMode(a, OUTPUT);       
  pinMode(gb2, OUTPUT);       
  pinMode(led, OUTPUT);       
  pinMode(b, OUTPUT);   
      
  pinMode(rdy, INPUT_PULLUP);    

  // GB2 is always ready to drive the bus
  digitalWrite(gb2,LOW);
  // G1 is used to drive the bus, idle for now
  digitalWrite(g1, LOW);
   
  Serial.begin(115200);
  Serial.println("TI speech synth code");

  // wait for ready
  while (digitalRead(rdy) == LOW);

  // LED will be lit when busy
  digitalWrite(led, LOW);

}

void loop() {
  Serial.println("resetting");
  do_write(CMD_RESET);
  delay(1000);

  Serial.println("Saying good (internal)");
  say_good_internal();
  delay(1000);
  
  Serial.println("Saying good (external)");
  say_good_external();
  delay(1000);

  Serial.println("Printing GOOD data from rom");
  uint32_t s = ADDRESS_GOOD;
  uint8_t c = 0;
  do_writeaddr(s);
  for(int i=0;i<0x24;i++) {
    c = do_read();
    Serial.print(c,HEX);
    Serial.print(" ");
  }
  Serial.println();
  delay(1000);
}

void say_good_internal()
{
  do_writeaddr(ADDRESS_GOOD);
  do_write(CMD_SPEAK);
}

void say_good_external()
{
  do_write(CMD_SPEAKEXT); 
  for(int i=0;i<SPEECH_GOOD_LEN;i++) {
    do_write(SPEECH_GOOD[i]); 
  }
}

void do_writeaddr(uint32_t a)
{
  do_write(CMD_LOADADDR | (a & 0x0f));
  do_write(CMD_LOADADDR | ((a & 0xf0)>>4));
  do_write(CMD_LOADADDR | ((a & 0xf00)>>8));
  do_write(CMD_LOADADDR | ((a & 0xf000)>>12));
  do_write(CMD_LOADADDR | ((a & 0x30000)>>16));
}

int do_read()
{
  digitalWrite(a,LOW);
  digitalWrite(b,HIGH);
  digitalWrite(g1,HIGH);
  // wait for not ready, goes low
  while (digitalRead(rdy) == HIGH);
  digitalWrite(led, HIGH);
  // wait for ready
  while (digitalRead(rdy) == LOW);
  // read the bus here
  int x=0;
  for (int i = 0;i<8;i++)
  {
    x <<= 1;
    if (digitalRead(data[i]) == HIGH) {
      x |= 0x01;
    }
  }
  // de-assert read
  digitalWrite(g1,LOW);
  digitalWrite(led, LOW);
  return x;
}

void do_write(int x)
{
  for (int i = 0;i<8;i++)
  {
    digitalWrite(data[i],((x & 0x80)>0)?HIGH:LOW);
    pinMode(data[i], OUTPUT);
    x <<= 1;
  }
  
  digitalWrite(a,HIGH);
  digitalWrite(b,HIGH);
  digitalWrite(g1,HIGH);
  // wait for not ready, goes low
  while (digitalRead(rdy) == HIGH);
  digitalWrite(led, HIGH);
  // wait for ready
  while (digitalRead(rdy) == LOW); 
  // deassert write 
  digitalWrite(g1,LOW);
  digitalWrite(led, LOW);

  for (int i = 0;i<8;i++)
  {
    pinMode(data[i], INPUT);
  }
}

