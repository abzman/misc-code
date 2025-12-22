/*
 driver for generic digit/segment multiplexed 7-segments
 written for bubble displays gotten from hamvention 2014
 by Evan Allen
 May 2014 
 */

int d = 4;

int segdisp[7] = {2,3,4,5,6,7,8}; // base 10 equivalents for digits 0~9
int posdisp[6] = {9,10,11,12,13,14}; // base 10 equivalents to close cathodes on display 0~3 on module
int numbers[17][7] = {{1,1,1,1,1,1,0},{0,0,0,0,1,1,0},{1,1,0,1,1,0,1},{1,1,1,1,0,0,1},{0,1,1,0,0,1,1},{1,0,1,1,0,1,1},{1,0,1,1,1,1,1},{1,1,1,0,0,0,0},{1,1,1,1,1,1,1},{1,1,1,0,0,1,1},{1,1,1,0,1,1,1},{0,0,1,1,1,1,1},{1,0,0,1,1,1,0},{0,1,1,1,1,0,1},{1,0,0,1,1,1,1},{1,0,0,0,1,1,1},{0,0,0,0,0,0,0}};
void setup()
{
  for(int i=0;i<7;i++)
  {
  pinMode(segdisp[i], OUTPUT);
  digitalWrite(segdisp[i], 0);
  }
  
  for(int i=0;i<6;i++)
  {
  pinMode(posdisp[i], OUTPUT);
  digitalWrite(posdisp[i],0);
  }
}

void displaynumber(int digit,int rawnumber)
// takes an integer and displays it on our 4-digit LED display module
{
    for(int i=0;i<7;i++)
  {
    digitalWrite(segdisp[i],numbers[rawnumber][i]);
  }
  
      for(int i=0;i<6;i++)
  {
    digitalWrite(posdisp[i],1);
  }
  
    digitalWrite(posdisp[digit],0);
  
}

void loop()
{
 /* for(int i =0;i<6;i++)
  {
    for(int j =0;j<16;j++)
    {
      displaynumber(i,j);
      delay(500);
    }
  }*/
  displaynumber(0,12);
  delay(d);
  displaynumber(1,13);
  delay(d);
  displaynumber(2,14);
  delay(d);
  displaynumber(3,15);
  delay(d);
  displaynumber(4,16);
  delay(d);
  displaynumber(5,16);
  delay(d);
}




















