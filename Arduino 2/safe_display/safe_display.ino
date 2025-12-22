//We always have to include the library
#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to the CLK 
 pin 10 is connected to LOAD 
 We have only a single MAX72XX.
 */
 #define SPI_CS 10
 #define SPI_MOSI 11
 #define SPI_CLK 13
LedControl lc=LedControl(SPI_MOSI,SPI_CLK,SPI_CS,1);

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;

void setup() {
  Serial.begin(115200);
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
}




  /*  

       ROW 1

         A
      ###########
      #         #
ROW 6 - F #    7    # B - ROW 2
      #    G    #
      ###########
      #         #
ROW 5 - E #         # C - ROW 3
      #         #
      ###########
           D
         ROW 4

  */  
// DigValues stores 8 byte for the display information
unsigned char DigValues[] = {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000};

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

void loop() { 
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



void SPI_SEND(unsigned char addr, char data) {

    digitalWrite(SPI_CS,LOW);
    shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,addr);
    shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,data);
    digitalWrite(SPI_CS,HIGH);
}
