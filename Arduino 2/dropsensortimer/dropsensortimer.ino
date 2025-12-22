/*
  Example: PrintModules

  This example shows how to manually print the list of modules present and the Base Controller firmware version.

  This example works with all P1000 Series modules

  This example will print out all modules that have signed on and the Base Controller firmware version.
  Any amount of modules between 1 and 15 can be used with this example.
   _____  _____  _____  _____  _____  _____  _____  _____  _____  _____  _____  _____  _____  _____  _____  _____
	|  P  ||  S  ||  S  ||  S  ||  S  ||  S  ||  S  ||  S  ||  S  ||  S  ||  S  ||  S  ||  S  ||  S  ||  S  ||  S  |
	|  1  ||  L  ||  L  ||  L  ||  L  ||  L  ||  L  ||  L  ||  L  ||  L  ||  L  ||  L  ||  L  ||  L  ||  L  ||  L  |
	|  A  ||  O  ||  O  ||  O  ||  O  ||  O  ||  O  ||  O  ||  O  ||  O  ||  O  ||  O  ||  O  ||  O  ||  O  ||  O  |
	|  M  ||  T  ||  T  ||  T  ||  T  ||  T  ||  T  ||  T  ||  T  ||  T  ||  T  ||  T  ||  T  ||  T  ||  T  ||  T  |
	|  -  ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     |
	|  1  ||  0  ||  0  ||  0  ||  0  ||  0  ||  0  ||  0  ||  0  ||  0  ||  1  ||  1  ||  1  ||  1  ||  1  ||  1  |
	|  0  ||  1  ||  2  ||  3  ||  4  ||  5  ||  6  ||  7  ||  8  ||  9  ||  0  ||  1  ||  2  ||  3  ||  4  ||  5  |
	|  0  ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     ||     |
	 ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯  ¯¯¯¯¯
   	Written by FACTS Engineering
	Copyright (c) 2019 FACTS Engineering, LLC
	Licensed under the MIT license.
*/

#include <P1AM.h>
#include <SerialCommand.h>
SerialCommand sCmd;
int beamTime = 0;
int beamTarget = 0;
int pulseStart = 0;

const int beamPin = 0;
const int triggerPin = 2;

void setup() { // the setup routine runs once:
  pinMode(SWITCH_BUILTIN, INPUT); //Set Swtich to be a digital input
  pinMode(beamPin, INPUT); //input from drop sensor, through fet
  pinMode(LED_BUILTIN, OUTPUT); //Set LED to be a digital output
  pinMode(triggerPin, OUTPUT); //output through fet to revpi
  attachInterrupt(digitalPinToInterrupt(beamPin), counting, CHANGE);
  
  Serial.begin(115200);  //initialize serial communication at 115200 bits per second
  while (!P1.init()) {
    ; //Wait for Modules to Sign on
  }

  // Setup callbacks for SerialCommand commands
  sCmd.addCommand("DROPSENSOR",   dropCount);          //whole number milliseconds for how long to watch the drop sensor for
  sCmd.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
  
  //Serial.println("Ready");
  digitalWrite(LED_BUILTIN, 1);
  digitalWrite(triggerPin, 1);
}
void loop() { // the loop routine runs over and over again forever:

  sCmd.readSerial();
  //digitalWrite(triggerPin, digitalRead(SWITCH_BUILTIN)); // Turn the LED on
  if (digitalRead(beamPin) == 0) //if pulse is happening
  {
    if ((beamTime + (millis() - pulseStart)) > beamTarget) //if the previous total recorded pulses plus the current part of the pulse exceeded the target
    {
      digitalWrite(triggerPin, 0); //tell revpi we are done
    }
  }
  else
  {
    if ((beamTime) > beamTarget) //if total recorded pulses are more than the target
    {
      digitalWrite(triggerPin, 0); //tell revpi we are done
    }
  }
}

void dropCount() {
  char *arg;
  arg = sCmd.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    beamTime = 0; //no beam time recorded yet
    digitalWrite(triggerPin, 1); //we are not there yet
    beamTarget = atoi(arg); //set target in ms
  }
  else {
  }
}

void counting() {
  if (digitalRead(beamPin) == 0)
  {
    pulseStart = millis();
    //Serial.println("start drop");
  }
  else
  {
    //Serial.println(millis() - pulseStart);
    beamTime += (millis() - pulseStart);
    //Serial.println(beamTime);
    //Serial.println("stop drop");
  }
}

//This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}
