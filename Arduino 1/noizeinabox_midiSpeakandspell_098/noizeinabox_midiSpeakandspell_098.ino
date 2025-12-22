// ======================================================================================= //
/*

  noizeboxmidi v0.98b
  Created by noizeinabox   @ gmail dot com  (c) 2012 http://noizeinabox.blogspot.com/

  Control a Speak and Spell or Speak And Read  with a Arduino
  [need a special interface check the website fo details]
  The sound results of Speak and spell will be strange sound like a bended speak and spell.

  This code is still in beta, so you may get some strange behaviours
  Remember that LOW and High state are inverted by the interface.

  LICENSE:
  This code is distributed under the GNU Public License
  which can be found at http://www.gnu.org/licenses/gpl.txt

  DISCLAIMER:
  The author is in no way responsible for any problems or damage caused by
  using this code. Use at your own risk.


  Copyright (c) 2011 by noizeInabox noizeinabox AT gmail.com
*/

// ======================================================================================= //
#include <SerialCommand328.h>
int ctl8pin = 6;                //CTL8 bus input (momentary pulse on this pin due to S&S screen and synth bus sharing)
int keyinput = 5;               //input for the keypad
int keyoutput = 7;              //output for the keypad
int ctl8lenght;                 //var to store the length of the ctl8 pulse
byte speechaddr[5];            //address to be sent to S&S (splitted in 5 nibble of 4 bit)
word Note = 0x004D;            //var for the midi note set to 0x004D by default
word programchg = 0xD300;      //var for the midi program change set to 0xD300 by default
word SSaddress = 0xD34D;       // temp var for the address to be sent to S&S
byte velocity;
// ======================================================================================= //
SerialCommand328 SCmd;   // The demo SerialCommand object
void setup()
{
  DDRB |=  B00111111;                    // port 8 to 13 as output
  PORTB = B00111111;                     //set output as LOW (needed for S&S otherwise he don't like it.)
  pinMode(keyinput, INPUT);              // set input for the keypad pin 4
  pinMode(ctl8pin, INPUT);                 // set input for ctl8 pin input
  pinMode(keyoutput, OUTPUT);            // set output for the keypad pin 2
  digitalWrite (keyoutput, HIGH);        // put keypad output to low ( without the keypas don't work)
  Serial.begin(115200);
  // Setup callbacks for SerialCommand commands
  SCmd.addCommand("P", process_command); // 
  SCmd.addCommand("S", step_command); // 
  SCmd.addDefaultHandler(speaknext);  // Handler for command that isn't matched  (says "What?")
}

// ======================================================================================= //
void loop()
{
  SCmd.readSerial();     // We don't do much, just process serial commands

}

void process_command()
{
  unsigned int aNumber;
  char *arg;

  Serial.println("We're in process_command");
  arg = SCmd.next();
  if (arg != NULL)
  {
    aNumber = atoi(arg);  // Converts a char string to an integer
    Serial.print("First argument was: ");
    Serial.println(aNumber, HEX);
    SSaddress = aNumber;
    addrsplit(SSaddress);              //call the function to slit the nibbles
    letscry (1);                       //make the baby cry ..
    presserase();                      // simulate a erase keypress
  }
  else {
    Serial.println("No arguments");
  }
}
void step_command()
{

  Serial.println("We're in step_command");
    Serial.print("address is: ");
    SSaddress += 1;
    Serial.println(SSaddress, HEX);
    addrsplit(SSaddress);              //call the function to slit the nibbles
    letscry (1);                       //make the baby cry ..
    presserase();                      // simulate a erase keypress
}


// ======================================================================================= //
void presserase () {
  while (((PIND & B00100000) >> keyinput) == LOW) { //wait until keyinput is low
  }

  if (((PIND & B00100000) >> keyinput) == HIGH) {            // check if keyinput is high
    PORTD &=  B01111111;                              // put keyoutput to 1 to create a pulse of 520 us
    delayMicroseconds(521);
    PORTD |=  B10000000;
  }
}
// ======================================================================================= //
void letscry (boolean speed)
{
  ctl8lenght = pulseIn(ctl8pin, HIGH);      // wait for the ctl8 pulse (to inject the command between the pulses)
  delayMicroseconds(10);                    // wait 10 us after the pulse
  PORTB = B00011111;                         // CS High [enable TMS]
  delayMicroseconds(5);

  PORTB = B00011111;                     // CS High / CTL Bus low / 1 clock   [reset TMS 0]
  delayMicroseconds(2);
  clk();

  for (int x = 0; x < 5; x++)             //load address nibble loop
  {
    PORTB = B00011101;                   //load address command (2)
    clk();
    PORTB = speechaddr[x];               //nibbles
    clk();
  }
  PORTB = B00010111;                         //read Rom Data (8)
  clk();
  if (speed == 1)                           //check the speak speed
  {
    PORTB = B00010101;                    //Speak command (10)
  }
  else
  {
    PORTB = B00011001;                    //Speak slow command (6)
  }
  clk();
  delayMicroseconds(2);

  PORTB = B00111111;                 //CS low [disable TMS]
}
// ======================================================================================= //
void speaknext ()
{
  ctl8lenght = pulseIn(ctl8pin, HIGH);      // wait for the ctl8 pulse (to inject the command between the pulses)
  delayMicroseconds(10);                    // wait 10 us after the pulse
  PORTB = B00011111;                         // CS High [enable TMS]
  delayMicroseconds(5);

  PORTB = B00011111;                     // CS High / CTL Bus low / 1 clock   [reset TMS 0]
  delayMicroseconds(2);
  clk();

  PORTB = B00010111;                         //read Rom Data (8)
  clk();
    PORTB = B00010101;                    //Speak command (10)
  clk();
  delayMicroseconds(2);

  PORTB = B00111111;                 //CS low [disable TMS]
}
// ======================================================================================= //

void clk()                                //create a clock of 160us = 6.2 khz
{
  PORTB |=  B00010000;
  delayMicroseconds(80);
  PORTB &= B00001111;
  delayMicroseconds(80);
}
// ======================================================================================= //
void addrsplit(word addr)
{
  speechaddr[0] = ((addr & 0x000F) + 0x10);
  speechaddr[1] = (((addr & 0x00F0) >> 4) + 0x10);
  speechaddr[2] = (((addr & 0x0F00) >> 8) + 0x10);
  speechaddr[3] = (((addr & 0xF000) >> 12) + 0x10);
  speechaddr[4] = B00011111 ;
}
// ======================================================================================= //
// This gets set as the default handler, and gets called when no other command matches.
void unrecognized()
{
  Serial.println("What?");
}
