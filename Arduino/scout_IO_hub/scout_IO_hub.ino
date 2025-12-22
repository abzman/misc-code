/*  receive a frame from can bus with mask and filter setting

    CAN Baudrate,
    
    #define CAN_5KBPS           1
    #define CAN_10KBPS          2
    #define CAN_20KBPS          3
    #define CAN_25KBPS          4 
    #define CAN_31K25BPS        5
    #define CAN_33KBPS          6
    #define CAN_40KBPS          7
    #define CAN_50KBPS          8
    #define CAN_80KBPS          9
    #define CAN_83K3BPS         10
    #define CAN_95KBPS          11
    #define CAN_100KBPS         12
    #define CAN_125KBPS         13
    #define CAN_200KBPS         14
    #define CAN_250KBPS         15
    #define CAN_500KBPS         16
    #define CAN_666KBPS         17
    #define CAN_1000KBPS        18
   
    CANBed V1: https://www.longan-labs.cc/1030008.html
    CANBed M0: https://www.longan-labs.cc/1030014.html
    CAN Bus Shield: https://www.longan-labs.cc/1030016.html
    OBD-II CAN Bus GPS Dev Kit: https://www.longan-labs.cc/1030003.html
   
*/
#include <SPI.h>
#include <mcp_canbus.h>

/* Please modify SPI_CS_PIN to adapt to your board.

   CANBed V1        - 17
   CANBed M0        - 3
   CAN Bus Shield   - 9
   CANBed 2040      - 9
   CANBed Dual      - 9
   OBD-2G Dev Kit   - 9
   OBD-II GPS Kit   - 9
   Hud Dev Kit      - 9

   Seeed Studio CAN-Bus Breakout Board for XIAO and QT Py - D7
*/


#define OUT_1 A0
#define OUT_2 A1
#define OUT_3 A2
#define OUT_4 A3

#define SPI_CS_PIN 9

MCP_CAN CAN(SPI_CS_PIN);  // Set CS pin

unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  // below code need for OBD-II GPS Dev Kit Atemga32U4 version
  pinMode(OUT_1, OUTPUT);
  pinMode(OUT_2, OUTPUT);
  pinMode(OUT_3, OUTPUT);
  pinMode(OUT_4, OUTPUT);
  digitalWrite(OUT_1, LOW);
  digitalWrite(OUT_2, LOW);
  digitalWrite(OUT_3, LOW);
  digitalWrite(OUT_4, LOW);

  // below code need for OBD-II GPS Dev Kit RP2040 version
  // pinMode(12, OUTPUT);
  // digitalWrite(12, HIGH);

  while (CAN_OK != CAN.begin(CAN_500KBPS))  // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS FAIL!");
    delay(100);
  }
  Serial.println("CAN BUS OK!");

  /*
     * set mask, set both the mask to 0x3ff
     */
  CAN.init_Mask(0, 0, 0x3ff);  // there are 2 mask in mcp2515, you need to set both of them
  CAN.init_Mask(1, 0, 0x3ff);


  /*
     * set filter, we can receive id from 0x04 ~ 0x09
     */
  CAN.init_Filt(0, 0, 0x10);  // there are 6 filter in mcp2515
  CAN.init_Filt(1, 0, 0x05);  // there are 6 filter in mcp2515
  CAN.init_Filt(2, 0, 0x06);  // there are 6 filter in mcp2515
  CAN.init_Filt(3, 0, 0x07);  // there are 6 filter in mcp2515
  CAN.init_Filt(4, 0, 0x08);  // there are 6 filter in mcp2515
  CAN.init_Filt(5, 0, 0x09);  // there are 6 filter in mcp2515
}

void loop() {
  if (CAN_MSGAVAIL == CAN.checkReceive())  // check if get data
  {
    CAN.readMsgBuf(&len, buf);  // read data,  len: data length, buf: data buf
                                /*
    Serial.println("\r\n------------------------------------------------------------------");
    Serial.print("Get Data From id: ");
    Serial.println(CAN.getCanId());
    for (int i = 0; i < len; i++)  // print the data
    {
      Serial.print("0x");
      Serial.print(buf[i], HEX);
      Serial.print("\t");
    }
    Serial.println();
*/

    if (CAN.getCanId() == 0x10) {
      //Serial.println("got it");
      digitalWrite(OUT_1, buf[0]);
      digitalWrite(OUT_2, buf[1]);
      digitalWrite(OUT_3, buf[2]);
      digitalWrite(OUT_4, buf[3]);

      delay(100);
      digitalWrite(OUT_1, 0); //solenoid

    }
  }
}

// END FILE
