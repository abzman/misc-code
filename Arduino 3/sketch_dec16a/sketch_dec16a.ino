int pulPin = 10;
int dirPin = 11;
int enblPin = 12;
int ledPin = 13;

int loops = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pulPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enblPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  digitalWrite(pulPin, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(enblPin, LOW);
  digitalWrite(dirPin, LOW);
  
  Serial.begin(115200);
  Serial.println("init");
  
  digitalWrite(enblPin, HIGH);
  delay(100);
  digitalWrite(enblPin, LOW);
}

void loop() {
  for(int i=0;i<400;i++)
  {
  // put your main code here, to run repeatedly: 
  digitalWrite(pulPin, HIGH);
 // digitalWrite(ledPin, HIGH);
  delay(1);
  digitalWrite(pulPin, LOW);
 // digitalWrite(ledPin, LOW);
  Serial.println("pulse");
  delay(1);
  }
  Serial.println("one revolution");
  delay(1000);
}
