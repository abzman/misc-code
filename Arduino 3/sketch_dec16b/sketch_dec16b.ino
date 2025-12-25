int PUL = 10; //define Pulse pin
int DIR = 11; //define Direction pin
int ENA = 12; //define Enable Pin
int LED = 9; //define LED Pin
int Button = 6; //define LED Pin
boolean Direction = 0; //current direction
void setup() {
  pinMode (PUL, OUTPUT);
  pinMode (DIR, OUTPUT);
  pinMode (ENA, OUTPUT);
  pinMode (LED, OUTPUT);
  pinMode (Button, INPUT);
  Serial.begin(115200);

  digitalWrite(ENA, 0); //0 to enable
  digitalWrite(LED, 0);
}

void loop() {
  /*
    Serial.println("forward");
    digitalWrite(DIR,1); //forward
    digitalWrite(LED,1);
    pulse(20000);

    Serial.println("stop");
    digitalWrite(LED,0);
    delay(2000);


    Serial.println("backward");
    digitalWrite(DIR,0); //backward
    digitalWrite(LED,1);
    pulse(20000);

    Serial.println("stop");
    digitalWrite(LED,0);
    delay(2000);
  */
  if (digitalRead(Button))
  {
    Direction = !Direction;
    if (Direction) {

      Serial.println("backward");
    }
    else {

      Serial.println("forward");
    }
    digitalWrite(DIR, Direction);
    digitalWrite(LED, 1);
    pulse(20000);

    Serial.println("stop");
    digitalWrite(LED, 0);
    delay(2000);
  }

}

void pulse(int pulses) {
  for (int i = 0; i < pulses; i++)
  {
    digitalWrite(PUL, 0);
    delayMicroseconds(50);
    digitalWrite(PUL, 1);
    delayMicroseconds(50);
  }
}

