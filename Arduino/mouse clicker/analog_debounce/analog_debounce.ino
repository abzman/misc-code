
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header
#include "Mouse.h"

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to

int sensorValue = 0;        // value read from the pot
int debounce = 20;

// LCD geometry
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
long timeNow = 0;



#include <Wire.h>
#include <RTC.h>

static DS1307 RTC;

void setup() {

  // initialize mouse control:
  Mouse.begin();
  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
  //while (!Serial) {
  // wait for serial port to connect. Needed for native USB port only
  // }
  RTC.begin();
  Serial.print("Is Clock Running: ");
  if (RTC.isRunning())
  {
    Serial.println("Yes");
    Serial.print(RTC.getDay());
    Serial.print("-");
    Serial.print(RTC.getMonth());
    Serial.print("-");
    Serial.print(RTC.getYear());
    Serial.print(" ");
    Serial.print(RTC.getHours());
    Serial.print(":");
    Serial.print(RTC.getMinutes());
    Serial.print(":");
    Serial.print(RTC.getSeconds());
    Serial.print("");
    if (RTC.getHourMode() == CLOCK_H12)
    {
      switch (RTC.getMeridiem()) {
        case HOUR_AM:
          Serial.print(" AM");
          break;
        case HOUR_PM:
          Serial.print(" PM");
          break;
      }
    }
    Serial.println("");
    Serial.println(RTC.getEpoch());
    delay(1000);
  }
  else
  {
    delay(1500);

    Serial.println("No");
    Serial.println("Setting Time");
    RTC.setHourMode(CLOCK_H24);
    RTC.setDateTime(__DATE__, __TIME__);
    Serial.println("New Time Set");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println(__TIME__);
    RTC.startClock();
  }


  int status;
  status = lcd.begin(LCD_COLS, LCD_ROWS);
  if (status) // non zero status means it was unsuccesful
  {
    hd44780::fatalError(status); // does not return
  }

  lcd.print("mouse timer");
  printTime();
}

void loop() {
  if (timeNow < 0)
    timeNow = 0;
  printTime();
  switch (readButtons()) {
    case 1:
      timeNow -= 60;
      break;
    case 2:
      timeNow += 60;
      break;
    case 3:
      timeNow -= 600;
      break;
    case 4:
      timeNow += 600;
      break;
    case 5:
      timeNow -= 3600;
      break;
    case 6:
      timeNow += 3600;
      break;
    case 7:
      timeNow -= 36000;
      break;
    case 8:
      timeNow += 36000;
      break;
    case 9:
      countdown();
      break;
    default:
      // statements
      break;
  }
  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(10);
}
void countdown()
{
  lcd.setCursor(0, 0);
  lcd.print("counting down");
  long realNow = RTC.getEpoch();
  while (timeNow > 0)
  {
    if (RTC.getEpoch() > realNow)
    {
      realNow = RTC.getEpoch();
      timeNow -= 1;
      printTime();
    }
    delay(100);
  }

  Mouse.press(MOUSE_LEFT);
  delay(500);
  Mouse.release(MOUSE_LEFT);

  lcd.setCursor(0, 0);
  lcd.print("done          ");
}
int readButtons()
{
  while (1)
  {
    sensorValue = analogRead(analogInPin);
    if (sensorValue < 467)
    {
      int i = 0;
      while (1)
      {
        sensorValue = analogRead(analogInPin);
        if (sensorValue < 467)
        {
          i++;
          if (i > debounce)
          {
            return 0;
          }
          delay(10);
          continue;
        }
        break;
      }
    }
    else if (sensorValue > 466 && sensorValue < 563)
    {
      int i = 0;
      while (1)
      {
        sensorValue = analogRead(analogInPin);
        if (sensorValue > 466 && sensorValue < 563)
        {
          i++;
          if (i > debounce)
          {
            return 1;
          }
          delay(10);
          continue;
        }
        break;
      }
    }
    else if (sensorValue > 562 && sensorValue < 644)
    {
      int i = 0;
      while (1)
      {
        sensorValue = analogRead(analogInPin);
        if (sensorValue > 562 && sensorValue < 644)
        {
          i++;
          if (i > debounce)
          {
            return 2;
          }
          delay(10);
          continue;
        }
        break;
      }
    }
    else if (sensorValue > 643 && sensorValue < 701)
    {
      int i = 0;
      while (1)
      {
        sensorValue = analogRead(analogInPin);
        if (sensorValue > 643 && sensorValue < 701)
        {
          i++;
          if (i > debounce)
          {
            return 3;
          }
          delay(10);
          continue;
        }
        break;
      }
    }
    else if (sensorValue > 700 && sensorValue < 743)
    {
      int i = 0;
      while (1)
      {
        sensorValue = analogRead(analogInPin);
        if (sensorValue > 700 && sensorValue < 743)
        {
          i++;
          if (i > debounce)
          {
            return 4;
          }
          delay(10);
          continue;
        }
        break;
      }
    }
    else if (sensorValue > 742 && sensorValue < 776)
    {
      int i = 0;
      while (1)
      {
        sensorValue = analogRead(analogInPin);
        if (sensorValue > 742 && sensorValue < 776)
        {
          i++;
          if (i > debounce)
          {
            return 5;
          }
          delay(10);
          continue;
        }
        break;
      }
    }
    else if (sensorValue > 775 && sensorValue < 802)
    {
      int i = 0;
      while (1)
      {
        sensorValue = analogRead(analogInPin);
        if (sensorValue > 775 && sensorValue < 802)
        {
          i++;
          if (i > debounce)
          {
            return 6;
          }
          delay(10);
          continue;
        }
        break;
      }
    }
    else if (sensorValue > 801 && sensorValue < 823)
    {
      int i = 0;
      while (1)
      {
        sensorValue = analogRead(analogInPin);
        if (sensorValue > 801 && sensorValue < 823)
        {
          i++;
          if (i > debounce)
          {
            return 7;
          }
          delay(10);
          continue;
        }
        break;
      }
    }
    else if (sensorValue > 822 && sensorValue < 840)
    {
      int i = 0;
      while (1)
      {
        sensorValue = analogRead(analogInPin);
        if (sensorValue > 822 && sensorValue < 840)
        {
          i++;
          if (i > debounce)
          {
            return 8;
          }
          delay(10);
          continue;
        }
        break;
      }
    }
    else if (sensorValue > 839 && sensorValue < 856)
    {
      int i = 0;
      while (1)
      {
        sensorValue = analogRead(analogInPin);
        if (sensorValue > 839 && sensorValue < 856)
        {
          i++;
          if (i > debounce)
          {
            return 9;
          }
          delay(10);
          continue;
        }
        break;
      }
    }
    else if (sensorValue > 855 && sensorValue < 999)
    {
      int i = 0;
      while (1)
      {
        sensorValue = analogRead(analogInPin);
        if (sensorValue > 855 && sensorValue < 999)
        {
          i++;
          if (i > debounce)
          {
            return 10;
          }
          delay(10);
          continue;
        }
        break;
      }
    }
    delay(10);
  }
  return 0;
}
void printTime()
{
  lcd.setCursor(0, 1);
  lcd.print(timeNow / 3600);
  lcd.print(":");
  lcd.print((timeNow / 60) % 60);
  lcd.print(":");
  lcd.print(timeNow % 60);
  lcd.print("     ");
}
