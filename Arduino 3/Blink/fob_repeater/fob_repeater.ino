int pinOut = 13;
int pinOut2 = 12;
void setup() {
  pinMode(pinOut, INPUT);
  pinMode(pinOut2, INPUT);
}

// the loop function runs over and over again forever
void loop() {


  //press button on fob a
  pinMode(pinOut, OUTPUT);
  digitalWrite(pinOut, LOW);

  //press button on fob b
  pinMode(pinOut2, OUTPUT);
  digitalWrite(pinOut2, LOW);

  //hold fob button down for
  delay(100);

  //release button on fob a
  pinMode(pinOut, INPUT);

  //release button on fob b
  pinMode(pinOut2, INPUT);

  //let fob button up for
  delay(500);



  
}
