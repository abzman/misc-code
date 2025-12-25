int LedPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A2};
int ButtonPin = A3;
int TriggerPin = A4;
int FocusPin = A5;

void setup() {                
  // initialize the digital pin as an output.
  pinMode(LedPins[0], OUTPUT);     
  pinMode(LedPins[1], OUTPUT);     
  pinMode(LedPins[2], OUTPUT);     
  pinMode(LedPins[3], OUTPUT);     
  pinMode(LedPins[4], OUTPUT);     
  pinMode(LedPins[5], OUTPUT);     
  pinMode(LedPins[6], OUTPUT);     
  pinMode(LedPins[7], OUTPUT);     
  pinMode(LedPins[8], OUTPUT);  
  pinMode(LedPins[9], OUTPUT);     
  pinMode(LedPins[10], OUTPUT);     
  pinMode(LedPins[11], OUTPUT);     
  pinMode(LedPins[12], OUTPUT);     
  pinMode(LedPins[13], OUTPUT);     
  pinMode(LedPins[14], OUTPUT);        
  pinMode(FocusPin, OUTPUT);     
  pinMode(TriggerPin, OUTPUT);     
  pinMode(ButtonPin, INPUT_PULLUP);     
}

// the loop routine runs over and over again forever:
void loop() {
  if(!digitalRead(ButtonPin))
  {
  countDown();
  }
}

void countDown() {
  
  digitalWrite(FocusPin, HIGH);
  for(int i=0;i<15;i++)
  {
  digitalWrite(LedPins[i], HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(300);
  if(i<10)
  {
  digitalWrite(FocusPin, LOW);
  }else{
    
  digitalWrite(FocusPin, HIGH);
  }
  digitalWrite(LedPins[i], LOW);   // turn the LED on (HIGH is the voltage level)
  }
  digitalWrite(FocusPin, LOW);
  digitalWrite(TriggerPin, HIGH);
  delay(300);
  digitalWrite(TriggerPin, LOW);

  digitalWrite(FocusPin, HIGH);
  for(int i=0;i<15;i++)
  {
  digitalWrite(LedPins[i], HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);
  if(i<10)
  {
  digitalWrite(FocusPin, LOW);
  }else{
    
  digitalWrite(FocusPin, HIGH);
  }
  digitalWrite(LedPins[i], LOW);   // turn the LED on (HIGH is the voltage level)
  }
  digitalWrite(FocusPin, LOW);
  digitalWrite(TriggerPin, HIGH);
  delay(300);
  digitalWrite(TriggerPin, LOW);

  digitalWrite(FocusPin, HIGH);
  for(int i=0;i<15;i++)
  {
  digitalWrite(LedPins[i], HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);
  if(i<10)
  {
  digitalWrite(FocusPin, LOW);
  }else{
    
  digitalWrite(FocusPin, HIGH);
  }
  digitalWrite(LedPins[i], LOW);   // turn the LED on (HIGH is the voltage level)
  }
  digitalWrite(FocusPin, LOW);
  digitalWrite(TriggerPin, HIGH);
  delay(300);
  digitalWrite(TriggerPin, LOW);
}

