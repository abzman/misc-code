void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (!digitalRead(6))
  {
    char button = 0x30;
    if (digitalRead(2))
    {
      button = button + 1;
    }
    if (digitalRead(3))
    {
      button = button + 2;
    }
    if (digitalRead(4))
    {
      button = button + 4;
    }
    if (digitalRead(5))
    {
      button = button + 8;
    }
    if (button == 0x3a)
    {
      button = 0x30;
    }
    Serial.println(button);
  }
}
