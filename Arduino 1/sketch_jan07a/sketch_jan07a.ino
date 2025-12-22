/*
 *  This sketch sends ads1115 current sensor data via HTTP POST request to thingspeak server.
 *  It needs the following libraries to work (besides the esp8266 standard libraries supplied with the IDE):
 *
 *  - https://github.com/adafruit/Adafruit_ADS1X15
 *
 *  designed to run directly on esp8266-01 module, to where it can be uploaded using this marvelous piece of software:
 *
 *  https://github.com/esp8266/Arduino
 *
 *  2015 Tisham Dhar
 *  licensed under GNU GPL
 */

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

//WIFI credentials go here
const char* ssid     = "i3detroit";
const char* password = "";
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

WiFiClient client;

double offsetI;
double filteredI;
double sqI,sumI;
int16_t sampleI;
double Irms;

double offsetV;
double filteredV;
double sqV,sumV;
int16_t sampleV;
double Vrms;

double squareRoot(double fg)  
{
  double n = fg / 2.0;
  double lstX = 0.0;
  while (n != lstX)
  {
    lstX = n;
    n = (n + fg / n) / 2.0;
  }
  return n;
}

double calcIrms(unsigned int Number_of_Samples)
{
  /* Be sure to update this value based on the IC and the gain settings! */
  float multiplier = 0.000335F;    /* ADS1115 @ +/- 4.096V gain (16-bit results) */
  for (unsigned int n = 0; n < Number_of_Samples; n++)
  {
    sampleI = ads.readADC_Differential_0_1();

    // Digital low pass filter extracts the 2.5 V or 1.65 V dc offset, 
  //  then subtract this - signal is now centered on 0 counts.
    offsetI = (offsetI + (sampleI-offsetI)/1024);
    filteredI = sampleI - offsetI;
    //filteredI = sampleI * multiplier;

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum 
    sumI += sqI;
  }
  
  Irms = squareRoot(sumI / Number_of_Samples)*multiplier; 

  //Reset accumulators
  sumI = 0;
//--------------------------------------------------------------------------------------       
 
  return Irms;
}

double calcVrms(unsigned int Number_of_Samples)
{
  /* Be sure to update this value based on the IC and the gain settings! */
  float multiplier = 0.00912F;    /* ADS1115 @ +/- 4.096V gain (16-bit results) */
  for (unsigned int n = 0; n < Number_of_Samples; n++)
  {
    sampleV = ads.readADC_Differential_2_3();

    // Digital low pass filter extracts the 2.5 V or 1.65 V dc offset, 
  //  then subtract this - signal is now centered on 0 counts.
    offsetV = (offsetV + (sampleV-offsetV)/1024);
    filteredV = sampleV - offsetV;
    //filteredI = sampleI * multiplier;

    // Root-mean-square method current
    // 1) square current values
    sqV = filteredV * filteredV;
    // 2) sum 
    sumV += sqV;
  }
  
  Vrms = squareRoot(sumV / Number_of_Samples)*multiplier; 

  //Reset accumulators
  sumV = 0;
//--------------------------------------------------------------------------------------       
 
  return Vrms;
}


void setup() {
  Serial.begin(115200);
  delay(10);
 
  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  ads.begin();
}

void loop() {
  
   

    double current = calcIrms(2048);
Serial.print("Current: ");
Serial.println(current);
  double voltage = calcVrms(2048);
Serial.print("Voltage: ");
Serial.println(voltage);

  //Serial.println("Waiting...");    
  // thingspeak needs minimum 15 sec delay between updates
  //delay(20000);  
}
