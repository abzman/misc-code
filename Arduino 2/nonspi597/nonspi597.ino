//  74HC597

#define DATA 12 // 74HC597 Q
#define LATCH 10 // 74HC597 /PL
#define SHIFT_CLK 13 // 74HC597 SHCP


void setup()   {
  pinMode(DATA, INPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  digitalWrite(LATCH, 1);
  digitalWrite(SHIFT_CLK, 0);
  Serial.begin(115200);
} // end setup

void loop()   {
  int i, j;
  uint8_t my_data_in = 0;

  digitalWrite(LATCH, 0);
  delay(1);
  digitalWrite(LATCH, 1);
  // load 8 bits from the 597 into my_data_in byte
  for (i = 7; i >= 0; i--)
  {
    // Toggle the clock to get the next bit


    // If data pin = 1 set it in my_data otherwise
    // do nothing since my_data_in is initialized to 0
    if (digitalRead(DATA)) { //temp) {
      my_data_in |= (1 << i);
      Serial.print("1");
    }
    else
    {
      Serial.print("0");
    }
    toggle_clock();
  }
  Serial.println();
}
// Turn the clock line 0 to 1
void toggle_clock()
{
  digitalWrite(SHIFT_CLK, 1);
  delayMicroseconds(200);
  digitalWrite(SHIFT_CLK, 0);
}
