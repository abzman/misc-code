int E = 6;        // enable, active low
int addr_pin[] = {2, 3, 4, 5};
int address = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pins as an output.
  for (int i = 0; i < sizeof(addr_pin) / sizeof(addr_pin[0]); ++i)
    pinMode(addr_pin[i], OUTPUT);
  pinMode(E, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(E, LOW);     // Set enable to HIGH

  for (int i = 0; i < sizeof(addr_pin) / sizeof(addr_pin[0]); ++i)
    digitalWrite(addr_pin[i], (address >> i) % 2);
  delay(100);
  address += 1;
  if (address > 15)
    address = 0;
}
