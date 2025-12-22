

#include <SerialCommand.h>
#include <Keypad.h>
#include "LedControl.h"
#include <EEPROM.h>

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
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'C', '0', 'R'}
};
byte rowPins[ROWS] = {A2, A3, A4, A5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {12, A0, A1}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

#define SPI_CS 10
#define SPI_MOSI 11
#define SPI_CLK 13
LedControl lc = LedControl(SPI_MOSI, SPI_CLK, SPI_CS, 1);

// DigValues stores 8 byte for the display information
unsigned char DigValues[] = {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000};

int start_address = 0;
int display_map[] = {1, 6, 2, 7};
int digit_map[] = {8, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11};
byte pin[4] = {15, 15, 15, 15}; //untypeable character, unlike 0
byte digit = 0; //index for pin[]
unsigned long current_time = millis();
unsigned long button_time = millis();
unsigned long timeout = 30000;

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
  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);


  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("OPEN",    open_lock);
  sCmd.addCommand("CLOSE",   close_lock);
  sCmd.addCommand("READ",   eeprom_read);
  sCmd.addCommand("READ_PIN",   read_pin);
  sCmd.addCommand("WRITE",   eeprom_write);
  sCmd.addCommand("WRITE_PIN",   write_pin);
  sCmd.addCommand("?",   help);
  sCmd.addCommand("HELP",   help);
  sCmd.addCommand("help",   help);
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")

}

void eeprom_read()
{
  int addr;
  char *arg;

  arg = sCmd.next();
  if (arg != NULL) {
    addr = atoi(arg);    // Converts a char string to an integer
    Serial.print("reading address: ");
    Serial.println(addr);
    Serial.println(EEPROM.read(addr));
  }
  else {
    Serial.println("No address");
  }
}

void eeprom_write()
{
  int addr;
  int data;
  char *arg;
  arg = sCmd.next();
  if (arg != NULL) {
    addr = atoi(arg);    // Converts a char string to an integer
  }
  else {
    Serial.println("No address");
  }

  arg = sCmd.next();
  if (arg != NULL) {
    data = atol(arg);
    Serial.print("wrote: ");
    Serial.print(data);
    Serial.print(" to address: ");
    Serial.println(addr);
    EEPROM.write(addr, data);
  }
  else {
    Serial.println("No data");
  }
}

void read_pin()
{
  Serial.print("current pin is: ");
  Serial.print(EEPROM.read(start_address));
  Serial.print(EEPROM.read(start_address + 1));
  Serial.print(EEPROM.read(start_address + 2));
  Serial.println(EEPROM.read(start_address + 3));
}
void write_pin()
{
  int data[4];
  char *arg;
  arg = sCmd.next();
  if (arg != NULL) {
    data[0] = atoi(arg);    // Converts a char string to an integer
  }
  else {
    Serial.println("No digits");
    return;
  }

  arg = sCmd.next();
  if (arg != NULL) {
    data[1] = atol(arg);
  }
  else {
    Serial.println("Too few digits");
    return;
  }

  arg = sCmd.next();
  if (arg != NULL) {
    data[2] = atol(arg);
  }
  else {
    Serial.println("Too few digits");
    return;
  }

  arg = sCmd.next();
  if (arg != NULL) {
    data[3] = atol(arg);
    EEPROM.write(start_address, data[0]);
    EEPROM.write(start_address + 1, data[1]);
    EEPROM.write(start_address + 2, data[2]);
    EEPROM.write(start_address + 3, data[3]);
    Serial.println("pin written to memory");
  }
  else {
    Serial.println("Too few digits");
    return;
  }
}

void transfer_pin()
{

  EEPROM.write(start_address, pin[0]);
  EEPROM.write(start_address + 1, pin[1]);
  EEPROM.write(start_address + 2, pin[2]);
  EEPROM.write(start_address + 3, pin[3]);
  close_lock();
  blank_pin();
}
void help()
{
  Serial.println("");
  Serial.println("Available commands:");
  Serial.println("OPEN (no args)");
  Serial.println("CLOSE (no args)");
  Serial.println("READ (address)");
  Serial.println("WRITE (address) (byte)");
  Serial.println("WRITE_PIN (digit 0) (digit 1) (digit 2) (digit 3)");
  Serial.println("READ_PIN (no args)");
  Serial.println("HELP (no args)");
  Serial.println("? (no args)");
  Serial.println("");
}
void loop() {
  //display_test();
  current_time = millis();
  char customKey = customKeypad.getKey();

  if (current_time - button_time > timeout)
  {
    blank_pin();
  }

  if (customKey) {
    if (customKey == 'C') {
      if (digit > 3)//full pin
      {
        transfer_pin();
      }
      blank_pin();
      close_lock();
    }
    else if (customKey == 'R')
    {
      blank_pin();
    }
    else {
      //Serial.println(customKey);
      button_time = millis();
      increment_pin(customKey);
    }
  }

  sCmd.readSerial();     // We don't do much, just process serial commands
  //display_test();
}

void blank_pin()
{
  Display(display_map[0], digit_map[10]);
  Display(display_map[1], digit_map[10]);
  Display(display_map[2], digit_map[10]);
  Display(display_map[3], digit_map[10]);
  for (int i = 0; i < 4; i++)
  {
    pin[i] = 15;
  }
  digit = 0;
}

void increment_pin(char c)
{
  Display(display_map[digit], digit_map[c - '0']);
  pin[digit] = c - '0';//stores as numbers rather than characters
  digit++;
  if (digit > 3)
  {
    if (!digitalRead(open_pin)) //open
    {
      //wait for close command
    } else
    {
      evaluate_pin();
    }
  }
}

void evaluate_pin()
{
  for (int i = 0; i < 3; i++)
  {
    if (EEPROM.read(start_address + i) != pin[i])
    {
      blank_pin();
      return;
    }
  }
  open_lock();
  blank_pin();
}

void display_test()
{

  /* Write data to display */
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 11; j++)
    {
      Display(display_map[i], digit_map[j]); //0-3, 0-10 (10 is blank)
      delay(1000);
    }
  }
}


void open_lock()
{
  if (!digitalRead(open_pin))
  {
    digitalWrite(dir_a, 1);
    digitalWrite(dir_b, 1);
    return;
  }
  digitalWrite(dir_a, 1);
  digitalWrite(dir_b, 0);
  while (digitalRead(open_pin))
  {
  }

  digitalWrite(dir_a, 1);
  digitalWrite(dir_b, 1);
  return;

}

void close_lock()
{
  if (!digitalRead(closed_pin))
  {
    digitalWrite(dir_a, 1);
    digitalWrite(dir_b, 1);
    return;
  }
  digitalWrite(dir_a, 0);
  digitalWrite(dir_b, 1);
  while (digitalRead(closed_pin))
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
  char numbers[] = {0b011111110, 0b00001100, 0b10110110, 0b10011110, 0b11001100, 0b11011010, 0b11111010, 0b00001110, 0b01111110, 0b11111110, 0b11011110, 0b00000000};

  // Dig positioning byte
  char bPosition = 0x00000001;

  // If [param]dig position is greater than 1
  if ( dig > 1) {
    // shift bPosition left to the correct digit ([param]dig -1 )
    bPosition = bPosition << dig - 1;
  }

  // iterate through the numbers array
  for (int x = 0; x < 8; x++) {
    // check if bit is high at the current location of our selected number
    if ( numbers[number] & (1 << x) )
    {
      // if bitX of numbers is high just binary OR bPosition to the current DigValues value
      DigValues[x] |= bPosition;
    } else {
      // if bitX of numbers is low, invert bPosition and binary AND it with current DigValues value to set bit low
      DigValues[x] = (0b11111111 ^ bPosition) & DigValues[x] ;
    }
    // Send your information to max7219
    SPI_SEND(x, DigValues[x]);
  }
}

void SPI_SEND(unsigned char addr, char data) {

  digitalWrite(SPI_CS, LOW);
  shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, addr);
  shiftOut(SPI_MOSI, SPI_CLK, MSBFIRST, data);
  digitalWrite(SPI_CS, HIGH);
}
