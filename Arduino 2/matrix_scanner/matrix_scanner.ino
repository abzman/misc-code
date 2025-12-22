#define Start_pin          26
#define Stop_pin           49
//this maps the arduino pins to whatever pins you're trying to decode
int pinMap[50]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,    28,0,1,0,2,0,3,0,7,0,8,0,9,0,10,0,11,0,12,0,13,0,14,0};

void setup() {
  //start serial connection
  Serial.begin(115200);
  //set all pins as inputs to be read
  for (int i = Start_pin; i < (Stop_pin + 1); i++)
  {
    pinMode(i, INPUT_PULLUP);
  }
}

void loop() {
  //iterate through pins, outputting on one at a time
  for (int i = Start_pin; i < (Stop_pin + 1); i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, 0);
    //delay(10);  //settling time?
    //iterate throughall pins reading them as inputs
    for (int j = Start_pin; j < (Stop_pin + 1); j++)
    {
      //if the output pin is not the same pin being read and there is a connection
      if (!digitalRead(j) && i != j)
      {
        //print connection
        Serial.print("found connection: ");
        Serial.print(pinMap[i]);
        Serial.print(" and ");
        Serial.println(pinMap[j]);
      }
    }
    //set pin back to input before moving on to the next pin
    pinMode(i, INPUT_PULLUP);

  }
}
