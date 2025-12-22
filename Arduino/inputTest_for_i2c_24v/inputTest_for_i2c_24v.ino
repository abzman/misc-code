/*
 KeyPressed on PIN1
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf8575-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF8575.h"

// Set i2c address
PCF8575 pcf8575(0x20);
unsigned long timeElapsed;
void setup() {
	Serial.begin(115200);

	pcf8575.pinMode(P0, INPUT);
	pcf8575.pinMode(P1, INPUT);
	pcf8575.pinMode(P2, INPUT);
	pcf8575.pinMode(P3, INPUT);
	pcf8575.pinMode(P4, INPUT);
	pcf8575.pinMode(P5, INPUT);
	pcf8575.pinMode(P6, INPUT);
	pcf8575.pinMode(P7, INPUT);
	pcf8575.pinMode(P8, INPUT);
	pcf8575.pinMode(P9, INPUT);
	pcf8575.pinMode(P10, INPUT);
	pcf8575.pinMode(P11, INPUT);
	pcf8575.pinMode(P12, INPUT);
	pcf8575.pinMode(P13, INPUT);
	pcf8575.pinMode(P14, INPUT);
	pcf8575.pinMode(P15, INPUT);

	pcf8575.begin();
}

void loop() {
	Serial.print(pcf8575.digitalRead(P0));
	Serial.print(pcf8575.digitalRead(P1));
	Serial.print(pcf8575.digitalRead(P2));
	Serial.print(pcf8575.digitalRead(P3));
	Serial.print(pcf8575.digitalRead(P4));
	Serial.print(pcf8575.digitalRead(P5));
	Serial.print(pcf8575.digitalRead(P6));
	Serial.print(pcf8575.digitalRead(P7));
	Serial.print(pcf8575.digitalRead(P8));
	Serial.print(pcf8575.digitalRead(P9));
	Serial.print(pcf8575.digitalRead(P10));
	Serial.print(pcf8575.digitalRead(P11));
	Serial.print(pcf8575.digitalRead(P12));
	Serial.print(pcf8575.digitalRead(P13));
	Serial.print(pcf8575.digitalRead(P14));
	Serial.println(pcf8575.digitalRead(P15));
	delay(100);
}
