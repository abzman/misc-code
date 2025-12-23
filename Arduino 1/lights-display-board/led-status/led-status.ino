#include <ESP8266WiFi.h>
#include <PubSubClient.h>


WiFiClient wifiClient;
PubSubClient client(wifiClient);

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//NOTE: To put the badge into program mode (for use when programming via FTDI cable), first hold down "program", then push "reset".
//Once the board resets, you may let go of the program button.  It should now be ready to receive a program over its FTDI pins.

const char* ssid = "i3detroit-wpa";
const char* password = "i3detroit";
const char* mqtt_server = "10.13.0.22";

long lastReconnectAttempt = 0;
uint8_t MAC_array[6];
char MAC_char[18];
char topic_buf[120];
#define LED_DATA 13

//The pin for our "program" button, which does double duty as a simple method of user input.
#define BUTTON 0

//HOWTO: Battery check
#define BATT_CHECK_PIN 4
//Turn batt_check pin high
//Read ADC (A0)
//Divide that value by 243.6 for voltage.
//Turn batt_check pin low

//The number of seconds we wait in the AP config portal mode before giving up and rebooting.
#define PORTAL_TIMEOUT 180

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(25, LED_DATA, NEO_GRB + NEO_KHZ800);

void battery_check() {
  Serial.println("Checking battery");

  digitalWrite(BATT_CHECK_PIN, HIGH);
  int sensorValue = analogRead(A0);
  float voltage = sensorValue / 243.6;
  Serial.print("Battery voltage: ");
  Serial.println(voltage);
  digitalWrite(BATT_CHECK_PIN, LOW);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //Set up the pin for LED use.
  strip.begin();
  //Should clear out any colors that might be stuck on at startup.
  strip.show();

  //Allows us to use the "program" (top) button in our program.
  pinMode(BUTTON, INPUT_PULLUP);

  pinMode(BATT_CHECK_PIN, OUTPUT);
  digitalWrite(BATT_CHECK_PIN, LOW);
  battery_check();
  solidColor(strip.Color(37, 0, 65), 0);
}

void loop() {
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  }else{
    client.loop();
    //connected
  }
  setBrightness(60, 0);
  strip.show();

  ////NOTE: Here's where the code for the main program should start.  Go hog wild!
  //setBrightness(0, 0);
  //strip.show();
  //setBrightness(20, 0);

  //// Some example procedures showing how to display to the pixels:
  //colorWipe(strip.Color(127, 0, 0), 50); // Red
  //colorWipe(strip.Color(0, 127, 0), 50); // Green
  //colorWipe(strip.Color(0, 0, 127), 50); // Blue
  //colorWipe(strip.Color(255, 255, 255), 50); // White RGBW
  //// Send a theater pixel chase in...
  //theaterChase(strip.Color(127, 127, 127), 50); // White
  //theaterChase(strip.Color(127, 0, 0), 50); // Red
  //theaterChase(strip.Color(0, 0, 127), 50); // Blue

  //rainbow(20);
  //rainbowCycle(20);
  //theaterChaseRainbow(50);
}
void mqtt_handle_subscriptions() {
   client.publish("stat/i3/test/LED_Display", MAC_char);
   client.subscribe("stat/i3/commons/lights/+/POWER");
   client.publish("cmnd/i3/commons/lights/all/POWER", "");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  topic += 34;
  topic[3] = '\0';
  uint32_t c;

  if((char)payload[1] == 'N') {
    c = strip.Color(0, 127, 0);
  } else if ((char)payload[1] == 'F') {
    c = strip.Color(127, 0, 0);
  } else {
    Serial.println("NOT A THING FUCK");
  }
  strip.setPixelColor(atoi(topic)-1, c);

}

boolean reconnect() {
  if(WiFi.status() != WL_CONNECTED) {
    //Wifi is disconnected
    Serial.println("WIFI BROKEN?");
  }
  int i = 0;
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // client.connect(mqtt node name)
    if (client.connect(MAC_char)) {
      Serial.println("connected");
      Serial.println(client.connected());
      // Once connected, publish an announcement...
      mqtt_handle_subscriptions();
      Serial.println(client.connected());
      //TODO: subscribe to multiple channels based on config
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  return client.connected();
}
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (i++ > 15) {
      Serial.println("Can't connect to wifi");
    }
    delay(500);
    Serial.print(".");
  }

  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array); ++i){
    sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
  }
  MAC_char[strlen(MAC_char)-1] = '\0';

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("MAC address: ");
  Serial.println(MAC_char);
}


void solidColor(uint32_t c, int wait) {
  for(uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void setBrightness(uint16_t b, int wait) {
  strip.setBrightness(b);
  strip.show();
  delay(wait);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
