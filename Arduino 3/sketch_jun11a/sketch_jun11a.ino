#define pin 14
void setup()
{
  Serial.begin(115200);
  Serial.println("testing pins on rgbcw led");
    pinMode(pin, OUTPUT);
    Serial.println("asserted as output");
    delay(1000);
}

void loop()
{

Serial.print("pin #");
Serial.print(pin);
Serial.println(" is high");
    digitalWrite(0, HIGH);

    delay(1000);

Serial.print("pin #");
Serial.print(pin);
Serial.println(" is low");

    digitalWrite(0, LOW);

    delay(1000);

}
