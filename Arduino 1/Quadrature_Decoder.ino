
#include <Encoder.h>

long previousMillis = 0;
long interval = 999;  // Update counter every second

Encoder rotaryEncoder(2, 3); // Select the two hardware interrupt pins on Arduino Uno

void setup() {
  Serial.begin(115200);
  Serial.println("Rotary Encoder Speed Test:");
}

long positionEncoder  = -999;
unsigned long myTime;

void loop() {
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;   

    long newEncoder;
    newEncoder = rotaryEncoder.read();
    
    Serial.print("Time: ");
    myTime = millis();
    Serial.print(myTime/1000); //prints time since program started
    Serial.println("s");
    
    Serial.print("Pulse Counter = ");
    Serial.println(newEncoder); // Because of quadrature, amount of pulses recieved by the encoder is multiplied by 4. It triggers on every rising and falling edge of the input signals.
    Serial.println("-------------------------");
    
    positionEncoder = newEncoder;
    rotaryEncoder.write(0);     // Measure amount of pulses counted for each second.     
  }
  
  if (Serial.available()) {
    Serial.read();
    rotaryEncoder.write(0);
  } 
}
