int pinArray[] = {10, 9, 8, 6, 5, 3};
int count = 0;
int timer = 20;
 
void setup(){
  for (count=0;count<6;count++) {
    pinMode(pinArray[count], OUTPUT);
  }
}
 
void loop() {
  for (count=0;count<5;count++) { digitalWrite(pinArray[count], HIGH); delay(timer); digitalWrite(pinArray[count + 1], HIGH); delay(timer); digitalWrite(pinArray[count], LOW); delay(timer*2); } for (count=5;count>0;count--) {
      digitalWrite(pinArray[count], HIGH);
      delay(timer);
      digitalWrite(pinArray[count - 1], HIGH);
      delay(timer);
      digitalWrite(pinArray[count], LOW);
      delay(timer*2);
    }
}
