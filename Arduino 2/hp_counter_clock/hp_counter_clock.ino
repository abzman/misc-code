void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("counter test program");
#define resetPin 12
#define clockPin 14
  pinMode(resetPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  digitalWrite(resetPin, HIGH);
  digitalWrite(clockPin, LOW);
  delay(100);
  // put your main code here, to run repeatedly:
  Serial.println("resetting");
  digitalWrite(resetPin, LOW);
  delay(100);
  digitalWrite(resetPin, HIGH);
  delay(100);
}

void loop() {
  
  
  Serial.println("clocking 10 in once per second");
  for (int i = 0; i < 59; i++)
  {
    Serial.println(i+1);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    delay(1000);
  }
  Serial.println("clocking 10 in 10 per second");
  for (int i = 0; i < 40; i++)
  {
    Serial.println(i+1);
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }

  /*
  Serial.println("clocking 10 in 100 per second");
  for (uint32_t i = 0; i < 1111111; i++)
  {
    digitalWrite(clockPin, HIGH);
    //delayMicroseconds(1);
    digitalWrite(clockPin, LOW);
    //delayMicroseconds(1);
  }
  delay(1000);
  */

}
