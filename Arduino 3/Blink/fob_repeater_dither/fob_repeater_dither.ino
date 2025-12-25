#define pinOut 13
#define pinOut2 12
#define maxOffset 2000
int currentOffset = 0;

void setup() {
  pinMode(pinOut, INPUT);
  pinMode(pinOut2, INPUT);
  //Serial.begin(115200);
}

// the loop function runs over and over again forever
void loop() {
    delay(499);
  if (currentOffset < (maxOffset/2))
  {
    /*
    Serial.print("a before b by ");
    Serial.print(currentOffset);
    Serial.println(" microseconds");
    */
    
    //press button on fob a
    pinMode(pinOut, OUTPUT);
    digitalWrite(pinOut, LOW);
    delayMicroseconds(currentOffset);
    //press button on fob b
    pinMode(pinOut2, OUTPUT);
    digitalWrite(pinOut2, LOW);

    //hold fob button down for
    delay(99);

    //release button on fob a
    pinMode(pinOut, INPUT);
    delayMicroseconds(currentOffset);
    //release button on fob b
    pinMode(pinOut2, INPUT);

  }
  else //fires at currentOffset=(maxOffset/2) or greater
  {
    /*
    Serial.print("b before a by ");
    Serial.print(currentOffset-(maxOffset/2));
    Serial.println(" microseconds");
    */

    //press button on fob b
    pinMode(pinOut2, OUTPUT);
    digitalWrite(pinOut2, LOW);
    delayMicroseconds(currentOffset-(maxOffset/2));
    //press button on fob a
    pinMode(pinOut, OUTPUT);
    digitalWrite(pinOut, LOW);
    
    //hold fob button down for
    delay(99);


    //release button on fob b
    pinMode(pinOut2, INPUT);
    delayMicroseconds(currentOffset-(maxOffset/2));
    //release button on fob a
    pinMode(pinOut, INPUT);


  }
  currentOffset++;
  if(currentOffset>maxOffset)
  {
  currentOffset = 0;
  }
}
