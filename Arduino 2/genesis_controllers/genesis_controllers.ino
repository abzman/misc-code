/*
 * Sega Controller Reader
 * Author: Jon Thysell &amp;lt;thysell@gmail.com&amp;gt;
 * Version: 1.0
 * Date: 7/26/2014
 *
 * Reads buttons presses from Sega Genesis 3/6 button controllers
 * and reports their state via the Serial connection. Handles hot
 * swapping of controllers and auto-switches between 3 and 6 button
 * polling patterns.
 *
 */
 #include <RoxMux.h>
// Controller Button Flags
const int ON = 1;
const int UP = 2;
const int DOWN = 4;
const int LEFT = 8;
const int RIGHT = 16;
const int START = 32;
const int A = 64;
const int B = 128;
const int C = 256;
const int X = 512;
const int Y = 1024;
const int Z = 2048;
 
// Controller DB9 Pin 7 Mappings
const int SELECT[] = { 8, A1 };
 
typedef struct
{
  int player;
  int pin;
  int lowFlag;
  int highFlag;
  int pulse3Flag;
} input;
 
// Controller DB9 Pin to Button Flag Mappings
// First column is the controller index, second column
// is the Arduino pin that the controller's DB9 pin is
// attached to
input inputMap[] = {
  { 0,  2,  UP,    UP,     Z}, // P0 DB9 Pin 1
  { 0,  3,  DOWN,  DOWN,   Y}, // P0 DB9 Pin 2
  { 0,  4,  ON,    LEFT,   X}, // P0 DB9 Pin 3
  { 0,  5,  ON,    RIGHT,  0}, // P0 DB9 Pin 4
  { 0,  6,  A,     B,      0}, // P0 DB9 Pin 6
  { 0,  7,  START, C,      0}, // P0 DB9 Pin 9
  { 1,  9, UP,    UP,     Z}, // P1 DB9 Pin 1
  { 1,  10, DOWN,  DOWN,   Y}, // P1 DB9 Pin 2
  { 1,  11, ON,    LEFT,   X}, // P1 DB9 Pin 3
  { 1,  12, ON,    RIGHT,  0}, // P1 DB9 Pin 4
  { 1,  13, A,     B,      0}, // P1 DB9 Pin 6
  { 1,  A0, START, C,      0}  // P1 DB9 Pin 9
};
 
// Controller State
int currentState[] = { 0, 0 };
int lastState[] = { -1, -1 };
 
// Default to three-button mode until six-button connects
boolean sixButtonMode[] = { false, false };
 RoxMCP23018 <0x20> controller1;
 RoxMCP23018 <0x21> controller2;
void setup()
{
  delay(1000);
  controller1.begin(true);
  controller2.begin(true);
  controller1.pinMode(0, OUTPUT); // pin 0 = GPA0 of MCP2301X
  controller1.pinMode(1, OUTPUT); // pin 1 = GPA1 of MCP2301X
  controller1.pinMode(2, OUTPUT); // pin 2 = GPA2 of MCP2301X
  controller1.pinMode(3, OUTPUT); // pin 3 = GPA3 of MCP2301X
  controller1.pinMode(4, OUTPUT); // pin 4 = GPA4 of MCP2301X
  controller1.pinMode(5, OUTPUT); // pin 5 = GPA5 of MCP2301X
  controller1.pinMode(6, OUTPUT); // pin 6 = GPA6 of MCP2301X
  controller1.pinMode(7, OUTPUT); // pin 7 = GPA7 of MCP2301X
  controller1.pinMode(8,  OUTPUT); // pin 8  = GPB0 of MCP2301X
  controller1.pinMode(9,  OUTPUT); // pin 9  = GPB1 of MCP2301X
  controller1.pinMode(10, OUTPUT); // pin 10 = GPB2 of MCP2301X
  controller1.pinMode(11, OUTPUT); // pin 11 = GPB3 of MCP2301X
  controller1.pinMode(12, OUTPUT); // pin 12 = GPB4 of MCP2301X
  controller1.pinMode(13, OUTPUT); // pin 13 = GPB5 of MCP2301X
  controller1.pinMode(14, OUTPUT); // pin 14 = GPB6 of MCP2301X
  controller1.pinMode(15, OUTPUT); // pin 15 = GPB7 of MCP2301X
  controller2.pinMode(0, OUTPUT); // pin 0 = GPA0 of MCP2301X
  controller2.pinMode(1, OUTPUT); // pin 1 = GPA1 of MCP2301X
  controller2.pinMode(2, OUTPUT); // pin 2 = GPA2 of MCP2301X
  controller2.pinMode(3, OUTPUT); // pin 3 = GPA3 of MCP2301X
  controller2.pinMode(4, OUTPUT); // pin 4 = GPA4 of MCP2301X
  controller2.pinMode(5, OUTPUT); // pin 5 = GPA5 of MCP2301X
  controller2.pinMode(6, OUTPUT); // pin 6 = GPA6 of MCP2301X
  controller2.pinMode(7, OUTPUT); // pin 7 = GPA7 of MCP2301X
  controller2.pinMode(8,  OUTPUT); // pin 8  = GPB0 of MCP2301X
  controller2.pinMode(9,  OUTPUT); // pin 9  = GPB1 of MCP2301X
  controller2.pinMode(10, OUTPUT); // pin 10 = GPB2 of MCP2301X
  controller2.pinMode(11, OUTPUT); // pin 11 = GPB3 of MCP2301X
  controller2.pinMode(12, OUTPUT); // pin 12 = GPB4 of MCP2301X
  controller2.pinMode(13, OUTPUT); // pin 13 = GPB5 of MCP2301X
  controller2.pinMode(14, OUTPUT); // pin 14 = GPB6 of MCP2301X
  controller2.pinMode(15, OUTPUT); // pin 15 = GPB7 of MCP2301X
  
  // Setup input pins
  for (int i = 0; i < sizeof(inputMap) / sizeof(input); i++)
  {
    pinMode(inputMap[i].pin, INPUT);
    digitalWrite(inputMap[i].pin, HIGH);
  }
 
  // Setup select pins
  for (int i = 0; i < 2; i++)
  {
    pinMode(SELECT[i], OUTPUT);
    digitalWrite(SELECT[i], HIGH);
  }
 
  Serial.begin(115200);
}
 
void loop()
{
  readButtons();
  sendStates();
}
 
void readButtons()
{
  for (int i = 0; i < 2; i++)
  {
    resetState(i);
    if (sixButtonMode[i])
    {
      read6buttons(i);
    }
    else
    {
      read3buttons(i);
    }
  }
}
 
void resetState(int player)
{
  currentState[player] = 0;
}
 
void read3buttons(int player)
{
  // Set SELECT LOW and read lowFlag
  digitalWrite(SELECT[player], LOW);
 
  delayMicroseconds(20);
 
  for (int i = 0; i < sizeof(inputMap) / sizeof(input); i++)
  {
    if (inputMap[i].player == player && digitalRead(inputMap[i].pin) == LOW)
    {
      currentState[player] |= inputMap[i].lowFlag;
    }
  }
 
  // Set SELECT HIGH and read highFlag
  digitalWrite(SELECT[player], HIGH);
 
  delayMicroseconds(20);
 
  for (int i = 0; i < sizeof(inputMap) / sizeof(input); i++)
  {
    if (inputMap[i].player == player && digitalRead(inputMap[i].pin) == LOW)
    {
      currentState[player] |= inputMap[i].highFlag;
    }
  }
 
  // When a six-button first connects, it'll spam UP and DOWN,
  // which signals the game to switch to 6-button polling
  if (currentState[player] == (ON | UP | DOWN))
  {
    sixButtonMode[player] = true;
  }
  // When a controller disconnects, revert to three-button polling
  else if ((currentState[player] & ON) == 0)
  {
    sixButtonMode[player] = false;
  }
 
  delayMicroseconds(20);
}
 
void read6buttons(int player)
{
  // Poll for three-button states twice
  read3buttons(player);
  read3buttons(player);
 
  // After two three-button polls, pulse the SELECT line
  // so the six-button reports the higher button states
  digitalWrite(SELECT[player], LOW);
  delayMicroseconds(20);
  digitalWrite(SELECT[player], HIGH);
 
  for(int i = 0; i < sizeof(inputMap) / sizeof(input); i++)
  {
    if (inputMap[i].player == player && digitalRead(inputMap[i].pin) == LOW)
    {
      currentState[player] |= inputMap[i].pulse3Flag;
    }
  }
 
  delayMicroseconds(1000);
}
 
void sendStates()
{
  // Only report controller states if at least one has changed
  boolean hasChanged = false;
 
  for (int i = 0; i < 2; i++)
  {
    if (currentState[i] != lastState[i])
    {
      hasChanged = true;
    }
  }
 
  if (hasChanged)
  {
    controller1.digitalWrite(0, (currentState[1] & UP) == UP ? 0 : 1);
    controller1.digitalWrite(1, (currentState[1] & DOWN) == DOWN ? 0 : 1);
    controller1.digitalWrite(2, (currentState[1] & LEFT) == LEFT ? 0 : 1);
    controller1.digitalWrite(3, (currentState[1] & RIGHT) == RIGHT ? 0 : 1);
    controller1.digitalWrite(4, (currentState[1] & START) == START ? 0 : 1);
    controller1.digitalWrite(5, (currentState[1] & A) == A ? 0 : 1);
    controller1.digitalWrite(6, (currentState[1] & C) == C ? 0 : 1);
    controller1.digitalWrite(7, (currentState[1] & B) == B ? 0 : 1);
    controller1.digitalWrite(8, (currentState[1] & X) == X ? 0 : 1);
    controller1.digitalWrite(9, (currentState[1] & Y) == Y ? 0 : 1);
    controller1.digitalWrite(10, (currentState[1] & Z) == Z ? 0 : 1);
    controller2.digitalWrite(0, (currentState[0] & UP) == UP ? 0 : 1);
    controller2.digitalWrite(1, (currentState[0] & DOWN) == DOWN ? 0 : 1);
    controller2.digitalWrite(2, (currentState[0] & LEFT) == LEFT ? 0 : 1);
    controller2.digitalWrite(3, (currentState[0] & RIGHT) == RIGHT ? 0 : 1);
    controller2.digitalWrite(4, (currentState[0] & START) == START ? 0 : 1);
    controller2.digitalWrite(5, (currentState[0] & A) == A ? 0 : 1);
    controller2.digitalWrite(6, (currentState[0] & C) == C ? 0 : 1);
    controller2.digitalWrite(7, (currentState[0] & B) == B ? 0 : 1);
    controller2.digitalWrite(8, (currentState[0] & X) == X ? 0 : 1);
    controller2.digitalWrite(9, (currentState[0] & Y) == Y ? 0 : 1);
    controller2.digitalWrite(10, (currentState[0] & Z) == Z ? 0 : 1);
 
      
      lastState[0] = currentState[0];
      lastState[1] = currentState[1];

        controller1.update(1);
        controller2.update(1);
  }
}
