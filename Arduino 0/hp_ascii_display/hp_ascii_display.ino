/*
 driver for generic digit/ascii multiplexed 16-segments
 written for hp 1414 gotten from hamvention 2014
 by Evan Allen
 June 2014 
 */
int data_pin[7] = {
  2,3,4,5,6,7,8}; // data pins
int addr_pin[2] = {
  A0,A1}; // address pins

int write_pin = A2;
void setup()
{
  for(int i=0;i<7;i++)
  {
    pinMode(data_pin[i], OUTPUT);
    digitalWrite(data_pin[i], 0);
  }

  for(int i=0;i<2;i++)
  {
    pinMode(addr_pin[i], OUTPUT);
    digitalWrite(addr_pin[i],0);
  }
  pinMode(write_pin, OUTPUT);
  digitalWrite(write_pin,1);

}

void displaynumber(int digit,int rawnumber)
// takes an integer and displays it on our 4-digit LED display module
{
  for (int j = 0; j < sizeof(addr_pin) / sizeof(addr_pin[0]); ++j)
    digitalWrite(addr_pin[j], !(digit & (1 << j)));

  digitalWrite(write_pin,0);

  for (int j = 0; j < sizeof(data_pin) / sizeof(data_pin[0]); ++j)
    digitalWrite(data_pin[j], (rawnumber & (1 << j)));

  digitalWrite(write_pin,1);

  delay(1);
}

void loop()
{
  displaynumber(0,'A');
  displaynumber(1,'B');
  displaynumber(2,'C');
  displaynumber(3,'D');
}





















