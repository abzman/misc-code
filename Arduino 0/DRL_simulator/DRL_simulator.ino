/*
  Fading

  This example shows how to fade an LED using the analogWrite() function.

  The circuit:
  - LED attached from digital pin 9 to ground through 220 ohm resistor.

  created 1 Nov 2008
  by David A. Mellis
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Fading
*/

int buttonPin = 2;
int pwmPin1 = 5;
int pwmPin2 = 6;

int buttonState = 0;  // variable for reading the pushbutton status
void setup() {

  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  if (!digitalRead(buttonPin)) {
    buttonState++;
    if (buttonState > 3) {
      buttonState = 0;
    }
    delay(500);
  }
  switch (buttonState) {
    case 0:
      analogWrite(pwmPin1, 0);
      analogWrite(pwmPin2, 0);
      break;
    case 1:
      analogWrite(pwmPin1, 1);
      analogWrite(pwmPin2, 1);
      break;
    case 2:
      analogWrite(pwmPin1, 2);
      analogWrite(pwmPin2, 2);
      break;
    case 3:
      analogWrite(pwmPin1, 255);
      analogWrite(pwmPin2, 255);
      break;
    default:
      // statements
      break;
  }
}
