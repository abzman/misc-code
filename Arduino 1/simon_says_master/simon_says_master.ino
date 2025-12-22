/*
  Simon tones from Wikipedia
  - A (red, upper left) - 440Hz - 2.272ms - 1.136ms pulse
  - a (green, upper right, an octave higher than A) - 880Hz - 1.136ms,
  0.568ms pulse
  - D (blue, lower left, a perfect fourth higher than the upper left)
  587.33Hz - 1.702ms - 0.851ms pulse
  - G (yellow, lower right, a perfect fourth higher than the lower left) -
  784Hz - 1.276ms - 0.638ms pulse


  Started: 12-26-2012
  Spark Fun Electronics
  The SparkFun Simon Says game has been through dozens of revisions over the years. This header
  file attempts to map and support all the different hardware versions.
*/




// include the library code:
#include <SerialCommand328.h>

#define CHOICE_OFF      0 //Used to control LEDs
#define CHOICE_NONE     0 //Used to check buttons
#define CHOICE_RED  (1 << 0)
#define CHOICE_GREEN  (1 << 1)
#define CHOICE_BLUE (1 << 2)
#define CHOICE_YELLOW (1 << 3)


// LED pin definitions, these are Arduino pins, not ATmega pins
#define LED_RED     3
#define LED_GREEN   5
#define LED_BLUE    10
#define LED_YELLOW  13

// Button pin definitions
#define BUTTON_RED    2
#define BUTTON_GREEN  6
#define BUTTON_BLUE   9
#define BUTTON_YELLOW 12

// Buzzer pin definitions
#define BUZZER1  4
#define BUZZER2  7

/*************************************************
   Public Constants
 *************************************************/

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978


#define ENTRY_TIME_LIMIT   3000 //Amount of time to press a button before game times out. 3000ms = 3 sec


SerialCommand328 SCmd;   // The SerialCommand object

void setup()
{
  //Setup hardware inputs/outputs. These pins are defined in the hardware_versions header file

  //Enable pull ups on inputs
  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  pinMode(BUZZER1, OUTPUT);
  pinMode(BUZZER2, OUTPUT);



  Serial.begin(9600);
  SCmd.addCommand("LEDS", SCmd_led); // turns leds on
  SCmd.addCommand("TONE", SCmd_tone); // plays tone with duration (note name)
  //format: TONE;<note>;<duration in milliseconds>
  //SCmd.addCommand("WINNER", SCmd_winner); // plays winner animation with duration
  //format: WINNER;<duration in loops>
  //SCmd.addCommand("LOSER", SCmd_loser); // plays loser animation with duration
  //format: LOSER;<duration in loops>
  //SCmd.addCommand("BEEGEE", SCmd_beegee); // plays beegee animation with duration
  //format: BEEGEE;<duration in loops>
  //SCmd.addCommand("ATTRACT", SCmd_attract); // plays attract animation with duration
  //format: ATTRACT;<duration in loops>
  SCmd.addDefaultHandler(unrecognized);  // Handler for command that isn't matched  (does nothing)


}

void loop()
{

  SCmd.readSerial();     //process serial commands
  //play_winner();
  //play_beegees();
  // byte choice = wait_for_button(); // See what button the user presses
  //attractMode();

}

void SCmd_winner()
{
int loops = 0;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
  {
    loops = atoi(arg);
  }
  for (int i = 0; i < loops; i++)
  {
    play_winner();
  }
  setLEDs(CHOICE_OFF); // Turn off all LEDs
}

void SCmd_loser()
{
int loops = 0;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
  {
    loops = atoi(arg);
  }
  for (int i = 0; i < loops; i++)
  {
    play_loser();
  }
  setLEDs(CHOICE_OFF); // Turn off all LEDs
}

void SCmd_beegee()
{
int loops = 0;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
  {
    loops = atoi(arg);
  }
  for (int i = 0; i < loops; i++)
  {
    play_beegees();
  }
  setLEDs(CHOICE_OFF); // Turn off all LEDs
}


void SCmd_attract()
{
int loops = 0;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
  {
    loops = atoi(arg);
  }
  for (int i = 0; i < loops; i++)
  {
    attractMode();
  }
  setLEDs(CHOICE_OFF); // Turn off all LEDs
}

void SCmd_tone()
{
int dur = 0;
  char *arg;
  char *note;
  arg = SCmd.next();
  if (arg != NULL)
  {
    note = arg;
  }
  arg = SCmd.next();
  if (arg != NULL)
  {
    dur = atoi(arg);
  }
  tone(BUZZER2, note, dur);
}

void SCmd_led()
{
int dur = 0;
  char *arg;
  arg = SCmd.next();
  if (arg != NULL)
  {
    
    setLEDs((byte)atoi(arg));
  }
}


// This gets set as the default handler, and gets called when no other command matches.
void unrecognized()
{
  Serial.println("What?");
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//The following functions are related to game play only


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//The following functions control the hardware

// Lights a given LEDs
// Pass in a byte that is made up from CHOICE_RED, CHOICE_YELLOW, etc
void setLEDs(byte leds)
{
  if ((leds & CHOICE_RED) != 0)
    digitalWrite(LED_RED, HIGH);
  else
    digitalWrite(LED_RED, LOW);

  if ((leds & CHOICE_GREEN) != 0)
    digitalWrite(LED_GREEN, HIGH);
  else
    digitalWrite(LED_GREEN, LOW);

  if ((leds & CHOICE_BLUE) != 0)
    digitalWrite(LED_BLUE, HIGH);
  else
    digitalWrite(LED_BLUE, LOW);

  if ((leds & CHOICE_YELLOW) != 0)
    digitalWrite(LED_YELLOW, HIGH);
  else
    digitalWrite(LED_YELLOW, LOW);
}

// Wait for a button to be pressed.
// Returns one of LED colors (LED_RED, etc.) if successful, 0 if timed out
byte wait_for_button(void)
{
  long startTime = millis(); // Remember the time we started the this loop

  while ( (millis() - startTime) < ENTRY_TIME_LIMIT) // Loop until too much time has passed
  {
    byte button = checkButton();

    if (button != CHOICE_NONE)
    {
      toner(button, 150); // Play the button the user just pressed

      while (checkButton() != CHOICE_NONE) ; // Now let's wait for user to release button

      delay(10); // This helps with debouncing and accidental double taps

      return button;
    }

  }

  return CHOICE_NONE; // If we get here, we've timed out!
}

// Returns a '1' bit in the position corresponding to CHOICE_RED, CHOICE_GREEN, etc.
byte checkButton(void)
{
  if (digitalRead(BUTTON_RED) == 0) return (CHOICE_RED);
  else if (digitalRead(BUTTON_GREEN) == 0) return (CHOICE_GREEN);
  else if (digitalRead(BUTTON_BLUE) == 0) return (CHOICE_BLUE);
  else if (digitalRead(BUTTON_YELLOW) == 0) return (CHOICE_YELLOW);

  return (CHOICE_NONE); // If no button is pressed, return none
}

// Light an LED and play tone
// Red, upper left:     440Hz - 2.272ms - 1.136ms pulse
// Green, upper right:  880Hz - 1.136ms - 0.568ms pulse
// Blue, lower left:    587.33Hz - 1.702ms - 0.851ms pulse
// Yellow, lower right: 784Hz - 1.276ms - 0.638ms pulse
void toner(byte which, int buzz_length_ms)
{
  setLEDs(which); //Turn on a given LED

  //Play the sound associated with the given LED
  switch (which)
  {
    case CHOICE_RED:
      buzz_sound(buzz_length_ms, 1136);
      break;
    case CHOICE_GREEN:
      buzz_sound(buzz_length_ms, 568);
      break;
    case CHOICE_BLUE:
      buzz_sound(buzz_length_ms, 851);
      break;
    case CHOICE_YELLOW:
      buzz_sound(buzz_length_ms, 638);
      break;
  }

  setLEDs(CHOICE_OFF); // Turn off all LEDs
}

// Toggle buzzer every buzz_delay_us, for a duration of buzz_length_ms.
void buzz_sound(int buzz_length_ms, int buzz_delay_us)
{
  // Convert total play time from milliseconds to microseconds
  long buzz_length_us = buzz_length_ms * (long)1000;

  // Loop until the remaining play time is less than a single buzz_delay_us
  while (buzz_length_us > (buzz_delay_us * 2))
  {
    buzz_length_us -= buzz_delay_us * 2; //Decrease the remaining play time

    // Toggle the buzzer at various speeds
    digitalWrite(BUZZER1, LOW);
    digitalWrite(BUZZER2, HIGH);
    delayMicroseconds(buzz_delay_us);

    digitalWrite(BUZZER1, HIGH);
    digitalWrite(BUZZER2, LOW);
    delayMicroseconds(buzz_delay_us);
  }
}

// Play the winner sound and lights
void play_winner(void)
{
  setLEDs(CHOICE_GREEN | CHOICE_BLUE);
  winner_sound();
  setLEDs(CHOICE_RED | CHOICE_YELLOW);
  winner_sound();
  setLEDs(CHOICE_GREEN | CHOICE_BLUE);
  winner_sound();
  setLEDs(CHOICE_RED | CHOICE_YELLOW);
  winner_sound();
}

// Play the winner sound
// This is just a unique (annoying) sound we came up with, there is no magic to it
void winner_sound(void)
{
  // Toggle the buzzer at various speeds
  for (byte x = 250 ; x > 70 ; x--)
  {
    for (byte y = 0 ; y < 3 ; y++)
    {
      digitalWrite(BUZZER2, HIGH);
      digitalWrite(BUZZER1, LOW);
      delayMicroseconds(x);

      digitalWrite(BUZZER2, LOW);
      digitalWrite(BUZZER1, HIGH);
      delayMicroseconds(x);
    }
  }
}

// Play the loser sound/lights
void play_loser(void)
{
  setLEDs(CHOICE_RED | CHOICE_GREEN);
  buzz_sound(255, 1500);

  setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
  buzz_sound(255, 1500);

  setLEDs(CHOICE_RED | CHOICE_GREEN);
  buzz_sound(255, 1500);

  setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
  buzz_sound(255, 1500);
}

// Show an "attract mode" display while waiting for user to press button.
void attractMode(void)
{
    setLEDs(CHOICE_RED);
    delay(100);

    setLEDs(CHOICE_BLUE);
    delay(100);

    setLEDs(CHOICE_GREEN);
    delay(100);

    setLEDs(CHOICE_YELLOW);
    delay(100);
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// The following functions are related to Beegees Easter Egg only

// Notes in the melody. Each note is about an 1/8th note, "0"s are rests.
int melody[] = {
  NOTE_G4, NOTE_A4, 0, NOTE_C5, 0, 0, NOTE_G4, 0, 0, 0,
  NOTE_E4, 0, NOTE_D4, NOTE_E4, NOTE_G4, 0,
  NOTE_D4, NOTE_E4, 0, NOTE_G4, 0, 0,
  NOTE_D4, 0, NOTE_E4, 0, NOTE_G4, 0, NOTE_A4, 0, NOTE_C5, 0
};

int noteDuration = 115; // This essentially sets the tempo, 115 is just about right for a disco groove :)
int LEDnumber = 0; // Keeps track of which LED we are on during the beegees loop

void play_beegees()
{
  setLEDs(CHOICE_NONE); // Turn off LEDs
  digitalWrite(BUZZER1, LOW); // setup the "BUZZER1" side of the buzzer to stay low, while we play the tone on the other pin.

    // iterate over the notes of the melody:
    for (int thisNote = 0; thisNote < 32; thisNote++) {
      changeLED();
      tone(BUZZER2, melody[thisNote], noteDuration);
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noTone(BUZZER2);
    }
}

// Each time this function is called the board moves to the next LED
void changeLED(void)
{
  setLEDs(1 << LEDnumber); // Change the LED

  LEDnumber++; // Goto the next LED
  if (LEDnumber > 3) LEDnumber = 0; // Wrap the counter if needed
}



