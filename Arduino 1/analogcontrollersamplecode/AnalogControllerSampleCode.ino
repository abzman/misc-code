/******************************************************************************/
/* ANALOG CONTROLLER A SAMPLE PROGRAM                                         */
/*                                                                            */
/*               CONTROLLER FOR ANALOG OUTPUTS AND INPUTS                     */
/*                                                                            */
/*               (C) DR.-ING. WOLFGANG GRIEBEL 2022                           */
/*                                                                            */
/*               VERSION 5.2 ARDUINO                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* LICENSE (GPL V3)                                                           */
/*                                                                            */
/* This program is free software: you can redistribute it and/or modify it    */
/* under the terms of the GNU General Public License as published by the Free */
/* Software Foundation, either version 3 of the License, or (at your option)  */
/* any later version.                                                         */
/* This program is distributed in the hope that it will be useful, but        */
/* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY */
/* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License    */
/* for more details.                                                          */
/* You should have received a copy of the GNU General Public License along    */
/* with this program. If not, see <https://www.gnu.org/licenses/>             */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* REVISION HISTORY                                                           */
/*                                                                            */
/* WHEN     WHAT                                                       WHO    */
/* -------- ---------------------------------------------------------- ----   */
/* 20220207 New ...                                                    DRWG   */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* Hardware Setup:                                                            */
/*                                                                            */
/* A0..A6 - Analog input / Digital IO as needed                               */
/* 0,1    - RX, TX not used                                                   */
/* 2-7    - 16x4LCD in 6 Bit connection                                       */
/* 8-13   - Driver ports for MCP4922 DACs                                     */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* Principal functionality (V1.0)                                             */
/*                                                                            */
/* The hardware to be controlled consists of                                  */
/* - up to 6 analog inputs                                                    */
/* - one LC display with 4 lines a 16 characters                              */
/* - 4 Digital outputs via MCP4922 DACs                                       */
/*                                                                            */
/* Front Panel Controls                                                       */
/* - LCD, LEDs if so desired                                                  */
/*                                                                            */
/* Back panel controls                                                        */
/* - none                                                                     */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/* EEPROM CONFIGURATION                                                       */
/* 4 Slots used                                                               */
/* Slot1: ANALOGCTRL=1             when true, further config data is assumed  */
/*     2: SERIALNO=xxx             serial number                              */
/*     3: DEBUGMODE=debugmode      debugging level, normally 0                */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/* Command supported:                                                         */
/*                                                                            */
/* STANDARD COMMANDS                                                          */
/* -------------------------------------------------------------------------- */
/* *RST                            Reset Instrument                           */
/* *RSTH                           Reset Instrument hard (reboot)             */ 
/* *CLS                            Clear last error                           */
/* *IDN?                           Who are you ?                              */
/* :SYST:ERR?                      Query last error                           */
/* *STB?                           Status Byte query                          */
/* *OPC?                           Operation complete query                   */
/* *DEBUG Mode                     Debugging accuracy (0=OFF)                 */
/* *DEBUG?                         Query debugging accuracy                   */
/* *LTEXT:SET Line,Text            Set display line                           */
/* *LTEXT?                         Query display line                         */
/* *NLINES?                        Return number of display lines             */
/* *GETSCREEN                      Get complete display                       */
/* *STDSYNTAX?                     Document standard syntax.                  */
/*                                 Multiline, ends with empty line.           */
/* *SYNTAX?                        Document syntax.                           */
/*                                 Multiline, ends with empty line.           */
/* *EECLEAR                        Clear EEPROM                               */
/* *EEWRITE Slot,Text              Write to an EEPROM Slot                    */
/* *EEREAD? Slot                   Read from an EEPROM Slot                   */
/* *EEDUMP?                        Dump all Slots                             */
/*                                                                            */
/* SPECIFIC COMMANDS                                                          */
/* -------------------------------------------------------------------------- */
/* :INSTR:PORTMODE:SET Port, (ANALOGINP | DIGITALINP | DIGITALOUT)            */
/*                                 Set mode of a port                         */
/* :INSTR:PORTMODE? Port           Query port mode                            */
/* :INSTR:PORTVAL:SET Port (ON | OFF)                                         */
/*                                 Set a port on/off in digital out mode      */
/* :INSTR:PORTVAL?                 Query port value in all modes              */
/* :INSTR:DACVAL:SET Port Value    Set DAC output value                       */
/* :INSTR:DACVAL? Port             Query DAC output value                     */
/* :INSTR:AUTOUPDATE:SET (ON | OFF)                                           */
/*                                 Set automatic display update ON or OFF     */
/* :INSTR:AUTOUPDATE?              Query automatic display update             */
/* :UPDATEALL                      Update all now                             */
/* :INSTR:AUTOTEXT:SET (ON | OFF)  Set automatic text ON/OFF                  */
/*                                                                            */
/* Remarks:                                                                   */
/* Unlike fully implemented SCPI, there is no command abbreviation and other  */
/* luxury. Commands must be written exactly as defined above.                 */
/* Lower/Uppercase mixing is allowed, however.                                */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* includes                                                                   */
/******************************************************************************/
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include <WString.h>

/******************************************************************************/
/* defines                                                                    */
/******************************************************************************/
const int rs = 7, en = 6, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/*----------------------------------------------------------------------------*/
/* configuration mode IMPORTANT sets all important behaviour                  */
/* sorry for the clumsy syntax because of dumb arduino preprocessor           */
/*----------------------------------------------------------------------------*/
#define MAXBUFLEN             64
#define BAUDRATE              38400

#define MAX_LCD_CHARACTERS    20
#define MAX_LCD_LINES         4

#define MAX_PORTS             6
#define MAX_DACPORTS          4

#define MANUFACTURER          "DL1DWG Technologies"
#define DEFAULT_SERIALNO      "SN1"
#define VERSIONINFO           "V5.2R 2022-02-07"

#define MINPORTNO             1
#define MAXPORTNO             6

#define MINDACPORTNO          1
#define MAXDACPORTNO          4

#define MINDACVAL             0.0
#define MAXDACVAL             5.0

#define MAX_EESLOTS           4
#define EESLOTLENGTH          64

#define MAX_PORTS             6
#define MAX_AUXPORTS          6

#define DOUBLE_NEEDED

#define NOERROR               (strlen(lastErrorText) == 0)

/******************************************************************************/
/* typedefs                                                                   */
/******************************************************************************/
enum PORTMODE {ANALOGINP,DIGITALINP,DIGITALOUT};

/******************************************************************************/
/* prototypes of local functions                                              */
/******************************************************************************/
void DisplaySettings ();
void writeToDac (int dacPortNo,int dacValue);
void formatOutString (int portNo, char *outString);

/******************************************************************************/
/* toolkit functions                                                          */
/******************************************************************************/
long int   GetNumToken(long int minVal, long int maxVal, const char* itemDesc);
#ifdef DOUBLE_NEEDED
double     GetDoubleToken(double minVal, double maxVal, const char* itemDesc);
#endif
char      *GetStringToken (const char* itemDesc);
bool       GetSwitchToken (const char* itemDesc);
char      *GetKeyToken (const char* itemDesc);
bool       HandleStandardCommands (char * token);
char      *GetEEPromStrKey (int slotNo, const char* keyName);
void       SoftwareReset (void);
void       HandleReset (void);

/******************************************************************************/
/* GLOBAL variables                                                           */
/******************************************************************************/
char      lastErrorText[MAXBUFLEN+1]        = "";
char      allowedChars[]                    = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-*/<>.:,;#=()%$§!? ";
bool      autoText                          = true;

char      serialNo[MAX_LCD_CHARACTERS+1]    = DEFAULT_SERIALNO;

char      idnStringS[MAXBUFLEN+1]           = "ARSAMPLE";
char      idnStringLC[MAXBUFLEN+1]          = "SAMPLECONTROLLER";

char      lineText[MAX_LCD_LINES][MAX_LCD_CHARACTERS+1]
                                            = {"","","",""};

PORTMODE  portModes[MAX_PORTS]              = {ANALOGINP,ANALOGINP,ANALOGINP,ANALOGINP,ANALOGINP,ANALOGINP};
bool      digitalPortValues[MAX_PORTS]      = {false,false,false,false,false,false};
int       analogPortValues[MAX_PORTS]       = {0,0,0,0,0,0};
int       dacPortValues[MAX_DACPORTS]       = {0,0,0,0};
int       portVector[MAX_PORTS]             = {A0,A1,A2,A3,A4,A5};
int       dacPortVector[3*MAX_DACPORTS/2]   = {8,9,10,11,12,13};

double    refVoltage                        = 5.0; // supplied externally from shield
double    resolutionADC                     = 1024;
double    resolutionDAC                     = 4096;

bool      autoUpdate                        = true;      // automatic update mode
int       upDateInterval                    = 1000;      // time in milliseconds

int       debugMode                         = 0;

char      auxString[MAXBUFLEN+1]            = "";

/******************************************************************************/
/* program space variables to save space in user RAM                          */
/******************************************************************************/
#define SYNTAXMESSAGE \
"Sample Controller Syntax\n\
  :INSTR:PORTMODE:SET Port, (ANALOGINP | DIGITALINP | DIGITALOUT)\n\
  :INSTR:PORTMODE? Port\n\
  :INSTR:PORTVAL:SET Port (ON | OFF)\n\
  :INSTR:PORTVAL?\n\
  :INSTR:DACVAL:SET Port Value\n\
  :INSTR:DACVAL? Port\n\
  :INSTR:AUTOUPDATE:SET (ON | OFF)\n\
  :INSTR:AUTOUPDATE?\n\
  :UPDATEALL\n\n"

#define STDSYNTAXMESSAGE \
"Sample Controller Standard Syntax\n\
  *RST\n\
  *RSTH\n\
  *CLS\n\
  *STB?\n\
  *OPC?\n\
  :SYST:ERR?\n\
  *LTEXT:SET Line, Text\n\
  *LTEXT? Line\n\
  *GETSCREEN?\n\
  *NLINES?\n\
  *EECLEAR\n\
  *EEWRITE Slot,Value\n\
  *EEREAD? Slot\n\
  *EEDUMP?\n\n"


// print a code space string literal
#define SERPRINTC(a)      { Serial.println(F(a)); }

// under debug, print a code space string literal
#define SERPRINTDBGC(a)   { if (debugMode > 0) Serial.println(F(a)); }

// under debug, print a normal string or other RAM variable.
#define SERPRINTDBGS(a)   { if (debugMode > 0) Serial.println(a); }

#define SETERR(a)         strcpy_P(lastErrorText,PSTR(a));

#define ISCMD(a)          (!strcmp_P(token,PSTR(a)))

#define ISKEY(a,b)        (!strcmp_P(a,PSTR(b)))

/******************************************************************************/
/* SETUP ROUTINE STARTS HERE                                                  */
/******************************************************************************/
void setup() {
  // opens serial port, sets baudrate
  Serial.begin(BAUDRATE); 

  // call reset routine
  HandleReset();

} /* setup */

/******************************************************************************/
/* reset routine                                                              */
/******************************************************************************/
void HandleReset() {

  // reinitialize common variables    
  autoUpdate        = true;      // automatic update mode
  upDateInterval    = 1000; 
  debugMode         = 0;  
    
  // initialize LCD (2..7), clear it and turn cursor off
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT); 
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT); 
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  lcd.begin(MAX_LCD_CHARACTERS, MAX_LCD_LINES);
  lcd.print(idnStringLC);
  lcd.setCursor(0, 1);
  lcd.print(VERSIONINFO);
  lcd.setCursor(0, 2);
  lcd.print("(C) Dipl.Ing.");
  lcd.setCursor(0, 3);
  lcd.print("W. Griebel 2020");
  delay(2000);
  lcd.begin(MAX_LCD_CHARACTERS, MAX_LCD_LINES);
  lcd.print("Setting all to");
  lcd.setCursor(0, 1);
  lcd.print("default values");
  delay(2000);
  DisplaySettings();

  // activate control ports for the ADCs
  pinMode(8, OUTPUT);   // SDI DAC 1 + 2
  pinMode(9, OUTPUT);   // SCK DAC 1 + 2
  pinMode(10,OUTPUT);   // CSQ DAC 1 + 2
  pinMode(11,OUTPUT);   // SDI DAC 3 + 4
  pinMode(12,OUTPUT);   // SCK DAC 3 + 4
  pinMode(13,OUTPUT);   // CSQ DAC 3 + 4

  // activate analog input ports
  for (int i = 0; i < MAX_PORTS; i++) {
    analogPortValues[i]  = 0;
    digitalPortValues[i] = 0;
    pinMode(portVector[i],INPUT);
  }

  for (int i = 0; i < MAX_DACPORTS; i++)
    dacPortValues[i] = 0;

  // activate DAC output ports (3 lines per chip for 2 ports)
  for (int i = 0; i < 3*MAX_DACPORTS/2; i++)
    pinMode(dacPortVector[i],OUTPUT);  

  // set reference. UNO/Leonardo resolution is 10bits.
  analogReference(EXTERNAL);

  // update DAC ports
  for (int i = 0; i < MAX_DACPORTS; i++)
    writeToDac(i,0);

  // do we have EEProm data available ? Try slot 1
  if (GetEEPromStrKey(1,"SAMPLECTRL") != NULL) {
    char  *keyValue = NULL;

    // we have an EEPROM data block. first check for debug mode
    if ((keyValue = GetEEPromStrKey(3,"DEBUGMODE")) != NULL)
      debugMode = atoi(keyValue);
    // get serial number from slot 2
    if ((keyValue = GetEEPromStrKey(2,"SERIALNO")) != NULL)
      strcpy(serialNo,keyValue);     
    else
      SERPRINTDBGC("No start config EEPROM data available.");
  } else
    SERPRINTDBGC("SAMPLECTRL Key not found. Proceeding with defaults.");   

  strcpy(lineText[0],idnStringLC);
  // display serial number
  lcd.begin(MAX_LCD_CHARACTERS, MAX_LCD_LINES);
  lcd.print("SERIAL NUMBER");
  lcd.setCursor(0, 1);
  lcd.print(serialNo);
  delay(2000);
  
  DisplaySettings();
} /* HandleReset */

/******************************************************************************/
/* MAIN PROGRAM STARTS HERE                                                   */
/******************************************************************************/
void loop() {
  int     charCount  = 0;
  char*   token      = NULL;
  char    inputString[MAXBUFLEN+1];
  int     i          = 0;
  int     j          = 0;
  int     portNo     = 0;
  int     auxPortNo  = 0;
  int     sleepCount = 0;

  // wait for incoming character
  while (Serial.available() == 0) {
    delay(1);
    if (autoUpdate) {
      sleepCount++;
      if (sleepCount > upDateInterval) {
        sleepCount = 0;
        strcpy(inputString,":UPDATEALL");
        break;
      }
    }
  } /* while */

  if (strcmp(inputString,":UPDATEALL")) {  
    // no updateall. we got something external. wait until incoming message is complete
    delay(50);  
    
    // send data only when you receive data:
    charCount       = min(Serial.available(),MAXBUFLEN-1);
    inputString[0]  = '\0';
      
    // compose input string, ignore invalid characters, convert to uppercase
    for (i = 0, j = 0; i < charCount; i++) {
      char inpChar  = Serial.read();
      inpChar       = *strupr(&inpChar);
      if (strchr(allowedChars,inpChar) != NULL)
        inputString[j++] = inpChar;
    }
    inputString[j] = '\0';
    strupr(inputString);
  }

  // tokenize and look for command. ignore empty input
  token = strtok(inputString,"; ");
  if ((token == NULL) || (strlen(token) == 0))
    return;

  // check standard commands first. If processing was done or in error, do nothing else
  if (HandleStandardCommands(token) || strlen(lastErrorText) > 0)
    ;

  // no standard command. classify tokens
  else if (ISCMD(":INSTR:PORTMODE:SET")) {
    PORTMODE  newPortMode = ANALOGINP;
    int       portNo      = 0;
    
    portNo = GetNumToken(MINPORTNO,MAXPORTNO,"PORTNO") - 1;
    if (NOERROR) {
      char  *portMode = NULL;
      
      portMode = GetKeyToken("PORTMODE");
      if (NOERROR) {
        if (ISKEY(portMode,"ANALOGINP"))
          newPortMode = ANALOGINP;
        else if (ISKEY(portMode,"DIGITALINP"))  
          newPortMode = DIGITALINP;
        else if (ISKEY(portMode,"DIGITALOUT"))  
          newPortMode = DIGITALOUT;
        else 
          SETERR("ERROR: Invalid Port Mode.");
      }      
    }
    if (NOERROR) {
      portModes[portNo]         = newPortMode;
      analogPortValues[portNo]  = 0;
      digitalPortValues[portNo] = false;
      if (newPortMode == ANALOGINP)
        pinMode(portVector[portNo],INPUT);
      else if (newPortMode == DIGITALINP)
        pinMode(portVector[portNo],INPUT_PULLUP);
      else if (newPortMode == DIGITALOUT) {      
        pinMode(portVector[portNo],OUTPUT);
        digitalWrite(portVector[portNo],0);
      }
    }
  }
  else if (ISCMD(":INSTR:PORTMODE?")) {
    int portNo = 0;
    
    Serial.println(token);
    portNo = GetNumToken(MINPORTNO,MAXPORTNO,"PORTNO") - 1;
    Serial.println(portNo);
    if (NOERROR) {
      switch (portModes[portNo]) {
        case ANALOGINP:
          SERPRINTC("ANALOGINP");
          break;
        case DIGITALINP:
          SERPRINTC("DIGITALINP");
          break; 
        case DIGITALOUT:
          SERPRINTC("DIGITALOUT");
          break;
        default:
          SETERR("ERROR: Invalid internal port mode.");
      }
    }               
  }
  else if (ISCMD(":INSTR:PORTVAL:SET")) {
    int portNo = 0;
    
    portNo = GetNumToken(MINPORTNO,MAXPORTNO,"PORTNO") - 1;
    if (NOERROR) {
      bool value = GetSwitchToken("STATE");
      
      if (NOERROR) {
        switch (portModes[portNo]) {
          case ANALOGINP:
            SETERR("ERROR: Port is analog INPUT.");
            break;
          case DIGITALINP:
             SETERR("ERROR: Port is digital INPUT.");
            break;
          case DIGITALOUT:
            digitalWrite(portVector[portNo],value);
            digitalPortValues[portVector[portNo]] = value;
            break;
        }
      }
    }                 
  }
  else if (ISCMD(":INSTR:PORTVAL?")) {
    int    portNo = 0;
    char   resultStr[16] = "";
    
    portNo = GetNumToken(MINPORTNO,MAXPORTNO,"PORTNO") - 1;
    if (NOERROR) {
      switch (portModes[portNo]) {
        case ANALOGINP:
          analogPortValues[portNo] = analogRead(portVector[portNo]);
          dtostrf(((double)analogPortValues[portNo])*refVoltage/resolutionADC,6,3,resultStr);
          Serial.println(resultStr);  
          break;
        case DIGITALINP:
          digitalPortValues[portNo] = digitalRead(portVector[portNo]);
          Serial.println(digitalPortValues[portNo] ? "HIGH" : "LOW");  
          break;
        case DIGITALOUT:
          Serial.println(digitalPortValues[portNo] ? "ON" : "OFF");       
          break;
      }
    }                 
  }
  else if (ISCMD(":INSTR:DACVAL:SET")) {
    int  dacPortNo = 0;
    
    dacPortNo = GetNumToken(MINDACPORTNO,MAXDACPORTNO,"DACPORTNO") - 1;
    if (NOERROR) {
      double  doubleSetValue = 0.;
        
      doubleSetValue = GetDoubleToken(MINDACVAL,MAXDACVAL,"DACVALUE");
      if (NOERROR) {
        dacPortValues[dacPortNo] = (int)((doubleSetValue*resolutionDAC)/refVoltage  + 0.5);
        dacPortValues[dacPortNo] = min(dacPortValues[dacPortNo],resolutionDAC-1);
        writeToDac(dacPortNo,dacPortValues[dacPortNo]);
      }
    }
  }
  else if (ISCMD(":INSTR:DACVAL?")) {
    int   dacPortNo     = 0;
    char  resultStr[16] = "";
    
    dacPortNo = GetNumToken(MINDACPORTNO,MAXDACPORTNO,"DACPORTNO") - 1;
    if (NOERROR) {
      dtostrf(((double)dacPortValues[portNo])*refVoltage/resolutionDAC,6,4,resultStr);
      Serial.println(resultStr); 
    }
  }
  else if (ISCMD(":INSTR:AUTOUPDATE?")) {
    Serial.println(autoUpdate ? "ON" : "OFF");
  }
  else if (!strcmp(token,":UPDATEALL")) {
    int   i;
    char  resultStr[16];

    // get all input/output port values
    for (i = 0; i < MAX_PORTS; i++) {
      switch (portModes[i]) {
        case ANALOGINP:
          analogPortValues[i] = analogRead(portVector[i]);
          dtostrf(((double)analogPortValues[i])*refVoltage/resolutionADC,6,3,resultStr);
          break;
        case DIGITALINP:
          digitalPortValues[i] = digitalRead(portVector[i]);
          break;
        case DIGITALOUT:
          break;
      }
    }
  }
  else {
    SETERR("ERROR: Unknown Command.");
    if (debugMode >= 0) {
      Serial.print(lastErrorText); 
      Serial.print(token);
      Serial.println("");
    }  
  }

  // display new settings
  strcpy(inputString,"");
  DisplaySettings();
      
} // main loop

/******************************************************************************/
/* display the settings that are active at the moment                         */
/******************************************************************************/
void DisplaySettings (void)
{
  char  lcdBuffer[MAX_LCD_LINES][MAX_LCD_CHARACTERS + 1];
  int   i = 0;
  int   j = 0;
  
  // clear display buffer
  memset(lcdBuffer,'\0',sizeof(lcdBuffer));

  if (autoText) {
    int  currLine       = 0;
    int  currPos        = 0;
    int  portNo         = 0;
    char outString[MAX_LCD_CHARACTERS + 1]  = "";
    double  dacResult1, dacResult2;
    char strResult1[MAX_LCD_CHARACTERS + 1];
    char strResult2[MAX_LCD_CHARACTERS + 1];
    
    // manually compose the display. Line 1 and 2 are output
    currLine = 0;
    dacResult1 = ((double)dacPortValues[0])*refVoltage/resolutionDAC;
    dacResult2 = ((double)dacPortValues[1])*refVoltage/resolutionDAC;
    dtostrf(dacResult1,5,3,strResult1);
    dtostrf(dacResult2,5,3,strResult2);
    sprintf(lcdBuffer[currLine],"DAC 1:%s  2:%s",strResult1,strResult2);
    currLine = 1;
    dacResult1 = ((double)dacPortValues[2])*refVoltage/resolutionDAC;
    dacResult2 = ((double)dacPortValues[3])*refVoltage/resolutionDAC;
    dtostrf(dacResult1,5,3,strResult1);
    dtostrf(dacResult2,5,3,strResult2);
    sprintf(lcdBuffer[currLine],"    3:%s  4:%s",strResult1,strResult2);
       
    // 3 and 4 are the analog/digital input lines
    currLine = 2;
    strcpy(lcdBuffer[currLine],"ADC 1:");
    currPos  = 6;
    for (portNo = 0; portNo < 3; portNo++) {
      formatOutString(portNo,outString);
      strcpy(lcdBuffer[currLine] + currPos,outString);
      currPos += 5;
    }
    currLine = 3;
    strcpy(lcdBuffer[currLine],"I/O 4:");
    currPos  = 6;
    for (portNo = 3; portNo < 6; portNo++) {
      formatOutString(portNo,outString);
      strcpy(lcdBuffer[currLine] + currPos,outString);
      currPos += 5;
    }
    // in autotext, copy the LCD lines back so they are SCPI readable.
    for (i = 0; i < MAX_LCD_LINES ; i++)
      memcpy(&lineText[i],&lcdBuffer[i],MAX_LCD_CHARACTERS+1);      
  } else {
    // no autoText. no fiddling, just copy the texts
    for (i = 0; i < MAX_LCD_LINES ; i++)
      strcpy(lcdBuffer[i],lineText[i]);
  }

  // replace zeros from the strcpy by blanks and write strings to LCD
  for (i = 0; i < MAX_LCD_LINES ; i++)
    for (j = 0; j < MAX_LCD_CHARACTERS; j++) {
      lcdBuffer[i][j] = lcdBuffer[i][j] ? lcdBuffer[i][j] : ' ';
      lineText[i][j]  = lineText[i][j]  ? lineText[i][j] : ' ';
    }
      
  // display all lines
  lcd.noCursor();
  lcd.begin(MAX_LCD_CHARACTERS, MAX_LCD_LINES);
  for (i = 0; i < MAX_LCD_LINES ; i++) {
    lcd.setCursor(0,i);
    lcd.print(lcdBuffer[i]);
  }
} /* DisplaySettings */

/******************************************************************************/
/* write a value to a DAC chip                                                */
/******************************************************************************/
void writeToDac(int dacPortNo, int dacValue)
{
  int           dataLine, clkLine, csqLine;
  unsigned int  sendInt;
  int           i;
  
  // all what comes is UGLY bitbanging because a normal Arduino has just one SPI port.
  // determine which lines to use
  dataLine = dacPortNo < 2 ? 8  : 11;
  clkLine  = dacPortNo < 2 ? 9  : 12;
  csqLine  = dacPortNo < 2 ? 10 : 13;

  // compose integer to send
  sendInt = (unsigned)dacValue;
  bitSet(sendInt,12);        // SHDNQ
  bitSet(sendInt,13);        // GAQ
  bitClear(sendInt,14);      // BUF
  bitClear(sendInt,15);      // A
  if ((dacPortNo % 2) == 1)
    bitSet(sendInt,15);

  // data and clock DOWN
  digitalWrite(dataLine,0);
  digitalWrite(clkLine,0);
  
  // CS activate
  digitalWrite(csqLine,0); 

  // bang bit for bit  
  for (i = 0; i < 16; i++) {
     digitalWrite(dataLine,bitRead(sendInt,15-i));
     delay(1);
     digitalWrite(clkLine,1);
     delay(1);
     digitalWrite(clkLine,0);   
  }
 
  // CS deactivate
  digitalWrite(csqLine,1);  
  
} /* writeToDac */

/******************************************************************************/
/* format an output string                                                    */
/******************************************************************************/
void formatOutString (int portNo, char *outString)
{
  char  resultStr[16] = "";
  
  switch (portModes[portNo]) {
    case ANALOGINP:
      dtostrf(((double)analogPortValues[portNo])*refVoltage/resolutionADC,4,2,resultStr);
      sprintf(outString,"%s",resultStr);
      break;
    case DIGITALINP:
      sprintf(outString,"%s",digitalPortValues[portNo] ? "HIGH" : "LOW");
      break;
    case DIGITALOUT:
      sprintf(outString,"%s",digitalPortValues[portNo] ? "ON" : "OFF");
      break;
  }
} /* formatOutString */

/******************************************************************************/
/* !!! COMMON TOOL LIBRARY STARTS HERE !!!                                    */
/* !!! COMMON TOOL LIBRARY STARTS HERE !!!                                    */
/* !!! COMMON TOOL LIBRARY STARTS HERE !!!                                    */
/******************************************************************************/
/******************************************************************************/
/* get a numeric token                                                        */
/******************************************************************************/
long int GetNumToken(long int minVal, long int maxVal, const char* itemDesc)
{
  char      *token = NULL;
  long int  intVal = 0;
    
  if (!NOERROR)                              // ignore previous error
    return 0;
  token = strtok(NULL,",");                   // uses las strtok operation
  if (token == NULL)                          // missing argument
    sprintf(lastErrorText,"%s missing.",itemDesc);
  else if (sscanf(token,"%ld",&intVal) != 1) 
    sprintf(lastErrorText,"%s Integer expected.",itemDesc);       
  else if ((intVal < minVal) || (intVal > maxVal))
    sprintf(lastErrorText,"%s out of range.",itemDesc);   

  return intVal;
} /* GetNumToken */

#ifdef DOUBLE_NEEDED
/******************************************************************************/
/* get a double token                                                         */
/******************************************************************************/
double GetDoubleToken (double minVal, double maxVal, const char* itemDesc)
{
  char    *token = NULL;
  double  dValue = 0.;
    
  if (!NOERROR)                               // ignore previous error
    return 0.;
  token = strtok(NULL,",");                   // uses las strtok operation
  if (token == NULL)                          // missing argument
    sprintf(lastErrorText,"%s missing.",itemDesc);
  else { 
    char *end = token;
    
    dValue = strtod(token, &end);
    if(end == token) 
      sprintf(lastErrorText,"%s Double expected.",itemDesc);       
    else if ((dValue < minVal) || (dValue > maxVal))
      sprintf(lastErrorText,"%s out of range.",itemDesc);
  }  

  return dValue;
} /* GetDoubleToken */
#endif

/******************************************************************************/
/* get a switch  token                                                        */
/******************************************************************************/
bool GetSwitchToken (const char* itemDesc)
{
  char  *token = NULL;
  char  auxString[MAXBUFLEN+1];
    
  if (!NOERROR)                               // ignore previous error
    return false;
  token = strtok(NULL,",");                   // uses last strtok operation
  if (token == NULL)                          // missing argument
    sprintf(lastErrorText,"%s missing.",itemDesc);
  else if (sscanf(token,"%s",auxString) != 1)
    sprintf(lastErrorText,"%s invalid",itemDesc);
  else if (strcmp(auxString,"ON") && strcmp(auxString,"OFF"))
    sprintf(lastErrorText,"%s invalid",itemDesc);
  else
    return (!strcmp(auxString,"ON"));
  return false;
} /* GetSwitchToken */

/******************************************************************************/
/* get a string token                                                         */
/******************************************************************************/
char *GetStringToken (const char* itemDesc)
{
  char  *token = NULL;

  memset(auxString,'\0',sizeof(auxString));   // null this out
  if (!NOERROR)                               // ignore previous error
    return NULL;
  token = strtok(NULL,"~");                   // uses last strtok operation
  if (token == NULL)                          // missing argument
    sprintf(lastErrorText,"%s missing.",itemDesc);
  else {
    strncpy(auxString,token,MAXBUFLEN);
    auxString[MAXBUFLEN] = '\0';
    return (auxString);
  }

  return NULL;
} /* GetStringToken */

/******************************************************************************/
/* get a keyword token                                                        */
/******************************************************************************/
char *GetKeyToken (const char* itemDesc)
{
  char  *token = NULL;

  memset(auxString,'\0',sizeof(auxString));   // null this out
  if (!NOERROR)                               // ignore previous error
    return NULL;
  token = strtok(NULL," ,");                  // uses last strtok operation
  if (token == NULL)                          // missing argument
    sprintf(lastErrorText,"%s missing.",itemDesc);
  else {
    strncpy(auxString,token,MAXBUFLEN);
    auxString[MAXBUFLEN] = '\0';
    return (Trim(auxString));
  }

  return NULL;
} /* GetKeyToken */

/******************************************************************************/
/* trim a string                                                              */
/******************************************************************************/
char *Trim (char* inpString)
{
  char  *startChar  = inpString;
  char  *endChar    = inpString + strlen(inpString);

  while ((endChar != startChar) && (isWhitespace(*endChar)))
    --endChar;
  while ((endChar != startChar) && (isWhitespace(*startChar)))
    ++startChar;
  return (startChar);
} /* Trim */

/******************************************************************************/
/* get a string token from an EEPROM Slot with a key                          */
/******************************************************************************/
char *GetEEPromStrKey (int slotNo, const char* keyName)
{
  char   *readName  = NULL;
  char   *readValue = NULL;
    
  // EEProm Read
  slotNo--;
  memset(auxString,'\0',sizeof(auxString));
  for (int i = 0; i < EESLOTLENGTH; i++)
    auxString[i] = EEPROM.read(i + slotNo*EESLOTLENGTH);

  readName = strtok(auxString,"=");
  if ((readName != NULL) && (!strcmp(readName,keyName))) {
    readValue = strtok(NULL," ");
    return readValue;
  }
  return (NULL);
} /* GetEEPromStrKey */

/******************************************************************************/
/* make a software reset                                                      */
/******************************************************************************/
void SoftwareReset (void)
{
  // create watchdog timer and let it run into an endless loop so it triggers.
  wdt_enable(WDTO_15MS);
  while (true)
    ;
} /* SoftwareReset */

/******************************************************************************/
/* handle standard commands                                                   */
/******************************************************************************/
bool HandleStandardCommands (char * token)
{
  // classify tokens. Standard commands first
  if (!strcmp(token,"*IDN?")) {
    // reply with ID string
    Serial.print(MANUFACTURER); Serial.print(",");
    Serial.print(idnStringS);   Serial.print(",");
    Serial.print(serialNo);     Serial.print(",");
    Serial.println(VERSIONINFO);
    return (true);
  }
  if (ISCMD("*RST")) {
    SERPRINTDBGC("Soft Resetting ...");
    HandleReset();
    return (true);
  }
  if (ISCMD("*RSTH")) {
    SERPRINTDBGC("Hard Resetting ...");
    SoftwareReset();
    return (true);
  }
  if (ISCMD("*CLS")) {
    strcpy(lastErrorText,"");
    return (true);
  }
  if (ISCMD(":SYST:ERR?") || ISCMD("SYST:ERR?")) {
    if (!NOERROR) {
      Serial.print("4,");
      Serial.println(lastErrorText);
    }
    else
      SERPRINTC("0, No Error.");
    return (true);
  }
  if (ISCMD("*STB?")) {
    SERPRINTC("0");
    return (true);
  }
  if (ISCMD("*OPC?")) {
    SERPRINTC("1");  
    return (true);  
  }
  if (ISCMD("*DEBUG")) {
    int  newDebugMode = 0;
    
    // get new debug mode
    newDebugMode   = GetNumToken(0,4,"DEBUG");
    if (strlen(lastErrorText) == 0) {
      // set new debug mode
      debugMode = newDebugMode;
      Serial.print("DEBUG "); 
      Serial.println(debugMode);
    }
    return (true);
  }
  if (ISCMD("*DEBUG?")) {
    Serial.println(debugMode);
    return (true);
  }
  if (ISCMD("*SYNTAX?")) {
    Serial.print(F(SYNTAXMESSAGE));
    return (true);
  }
  if (ISCMD("*STDSYNTAX?")) {
    Serial.print(F(STDSYNTAXMESSAGE));
    return (true);
  }
  if (ISCMD("*EECLEAR")) {
    // clear complete EEPROM. Disable interrupts while we do that
    noInterrupts();
    for (int i = 0; i < EEPROM.length(); i++)
      EEPROM.update(i,0);
    interrupts();
    return (true);      
  }
  if (ISCMD("*EEWRITE")) {
    char  *value = NULL;
    int   slotNo = 0;
    
    // EEProm Write. 
    slotNo   = GetNumToken(1,16,"SLOTNO") - 1;
    value    = GetStringToken("CONTENT");
    if (NOERROR) {
      // write to slot (byte by byte). Disable interrupts while we do that
      if (debugMode > 0) {
        SERPRINTC("EEWRITE ...");
        Serial.println(value);
      }
      noInterrupts();
      for (int i = 0; i < EESLOTLENGTH; i++)
        EEPROM.update(i + slotNo*EESLOTLENGTH,*(value + i));
      interrupts();  
    }
    return (true);  
  }
  if (ISCMD("*EEREAD?")) {
    int   slotNo = 0;
    char  auxString[MAXBUFLEN+1];
    
    // EEProm Read
    memset(auxString,'\0',sizeof(auxString));
    slotNo   = GetNumToken(1,16,"SLOTNO") - 1;
    if (NOERROR) {
      for (int i = 0; i < EESLOTLENGTH; i++)
        auxString[i] = EEPROM.read(i + slotNo*EESLOTLENGTH);
        Serial.println(auxString);
    }
    return (true); 
  }
  if (ISCMD("*EEDUMP?")) {
    char  auxString[MAXBUFLEN+1];

    for (int slotNo = 0; slotNo < MAX_EESLOTS; slotNo++) {
      for (int i = 0; i < EESLOTLENGTH; i++)
        auxString[i] = EEPROM.read(i + slotNo*EESLOTLENGTH);
      Serial.println(auxString);
    }
    Serial.print('\n');
    return (true); 
  }
  if (ISCMD("*LTEXT:SET")) {
    char  newLineText[MAXBUFLEN] = "";
    int   lineNo                 = 0;
    
    lineNo = GetNumToken(1,MAX_LCD_LINES,"LCDLINE") - 1;
    if (NOERROR) {
      char *newText = GetStringToken("TEXT");

      if (NOERROR) {
        strncpy(newLineText,newText,MAX_LCD_CHARACTERS);
        newLineText[MAX_LCD_CHARACTERS] = '\0';
        strcpy(lineText[lineNo],newLineText);
      }
    }
    return (true); 
  }
  if (ISCMD("*LTEXT?")) {
    int   lineNo = 0;
    
    lineNo = GetNumToken(1,MAX_LCD_LINES,"LCDLINES") - 1;
    if (NOERROR) {
      lineText[lineNo][MAX_LCD_CHARACTERS] = '\0';
      Serial.println(lineText[lineNo]);
    }
    return (true); 
  }
  if (ISCMD("*NLINES?")) {
    Serial.println(MAX_LCD_LINES);
    return (true); 
  }
  if (ISCMD("*GETSCREEN?")) {
    for (int i = 0; i < MAX_LCD_LINES; i++)
      Serial.println(lineText[i]);
    Serial.print('\n');
    return (true); 
  }
  return (false);
} /* HandleStandardCommands */
