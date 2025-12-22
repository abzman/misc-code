

#include <SerialCommand.h>
#include <Keypad.h>
#include "LedControl.h"

#define closed_pin 2
#define open_pin 3
#define dir_a 4
#define dir_b 5
#define speed_pin 6

SerialCommand sCmd;     // The demo SerialCommand object

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'C','0','R'}
};
byte rowPins[ROWS] = {A2, A3, A4, A5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {12, A0, A1}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

 #define SPI_CS 10
 #define SPI_MOSI 11
 #define SPI_CLK 13
LedControl lc=LedControl(SPI_MOSI,SPI_CLK,SPI_CS,1);

// DigValues stores 8 byte for the display information
unsigned char DigValues[] = {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000};

void setup() {
  // put your setup code here, to run once:
pinMode(closed_pin, INPUT_PULLUP);
pinMode(open_pin, INPUT_PULLUP);
pinMode(dir_a, OUTPUT);
pinMode(dir_b, OUTPUT);
pinMode(speed_pin, OUTPUT);
Serial.begin(115200);
  digitalWrite(dir_a, 1);
  digitalWrite(dir_b, 1);
analogWrite(speed_pin, 255);

  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);


  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("OPEN",    open_lock);          // Turns LED on
  sCmd.addCommand("CLOSE",   close_lock);         // Turns LED off
sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  
}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  if(!digitalRead(closed_pin))
  {
    Serial.println("closed");
  }
  if(!digitalRead(open_pin))
  {
    Serial.println("open");
  }
  */

  char customKey = customKeypad.getKey();
  
  if (customKey){
    Serial.println(customKey);
  }
  
  sCmd.readSerial();     // We don't do much, just process serial commands
display_test();
}

void display_test()
{
  int display_map[] = {1, 6, 2, 7};
int digit_map[] = {8, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11};

  /* Write data to display */
  for(int i=0;i<4;i++)
  {
    for(int j=0;j<11;j++)
    {
      Display(display_map[i],digit_map[j]);
      Serial.print("digit: ");
      Serial.print(i);
      Serial.print(" number: ");
      Serial.println(j);
      delay(1000);
    }
  }
}


void open_lock()
{
  if(!digitalRead(open_pin))
  {
  digitalWrite(dir_a, 1);
  digitalWrite(dir_b, 1);
    return;
  }
  digitalWrite(dir_a, 1);
  digitalWrite(dir_b, 0);
  while(digitalRead(open_pin))
  {
  }
  
  digitalWrite(dir_a, 1);
  digitalWrite(dir_b, 1);
    return;

}

void close_lock()
{
  if(!digitalRead(closed_pin))
  {
  digitalWrite(dir_a, 1);
  digitalWrite(dir_b, 1);
    return;
  }
  digitalWrite(dir_a, 0);
  digitalWrite(dir_b, 1);
  while(digitalRead(closed_pin))
  {
  }
  
  digitalWrite(dir_a, 1);
  digitalWrite(dir_b, 1);
    return;

}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}

// Display function
void Display(int dig, int number)
{
  // Array with information about which bit has to be set to display the correct number
  // numbers[0] -&gt; Displays 0
  // numbers[4] -&gt; Displays 4
  // ...
  char numbers[] = {0b011111110,0b00001100,0b10110110,0b10011110,0b11001100,0b11011010,0b11111010,0b00001110,0b01111110,0b11111110,0b11011110};

  // Dig positioning byte
  char bPosition = 0x00000001;

  // If [param]dig position is greater than 1
  if ( dig > 1){
    // shift bPosition left to the correct digit ([param]dig -1 )
    bPosition = bPosition<<dig-1;
  }

  // iterate through the numbers array
  for(int x = 0; x < 8; x++){
    // check if bit is high at the current location of our selected number
    if ( numbers[number] & (1 << x) )
    {
      // if bitX of numbers is high just binary OR bPosition to the current DigValues value
      DigValues[x] |= bPosition;
    }else{
      // if bitX of numbers is low, invert bPosition and binary AND it with current DigValues value to set bit low
      DigValues[x] = (0b11111111 ^ bPosition) & DigValues[x] ;
    }
    // Send your information to max7219
    SPI_SEND(x,DigValues[x]);
  }
}

void SPI_SEND(unsigned char addr, char data) {

    digitalWrite(SPI_CS,LOW);
    shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,addr);
    shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,data);
    digitalWrite(SPI_CS,HIGH);
}
