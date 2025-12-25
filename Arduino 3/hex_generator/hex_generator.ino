void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < 255; i++) {
    Serial.print("0x");
    Serial.print(i,HEX);
    Serial.print(",");
  }
  Serial.println();
  delay(5000);
}
