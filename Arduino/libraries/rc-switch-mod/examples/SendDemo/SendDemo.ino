/*
  Example for different sending methods
  
  https://github.com/sui77/rc-switch/
  
*/

#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {

  Serial.begin(9600);
  
  // Transmitter is connected to Arduino Pin #10  
  mySwitch.enableTransmit(D1);
  
  // Optional set protocol (default is 1, will work for most outlets)
   mySwitch.setProtocol(5);

  // Optional set pulse length.
   //mySwitch.setPulseLength(320);
  
  // Optional set number of transmission repetitions.
   mySwitch.setRepeatTransmit(15);
  
}

void loop() {

  /* Same switch as above, but using decimal code */
  mySwitch.send(13382, 15);//1 on
  delay(1000);  
  mySwitch.send(13350, 15);//1 off
  delay(1000);  
  mySwitch.send(13334, 15);//2 on
  delay(1000);  
  mySwitch.send(13326, 15);//2 off
  delay(1000);  
  mySwitch.send(13446, 15);//3 on
  delay(1000);  
  mySwitch.send(13574, 15);//3 off
  delay(1000);  
}
