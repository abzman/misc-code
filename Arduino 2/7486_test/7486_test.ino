const int in = A1;
const int out = A0;
void setup() {
pinMode (in, INPUT);
pinMode (out, OUTPUT);

}

void loop() {

  digitalWrite (out, digitalRead(in));

}
