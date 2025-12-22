uint32_t last_ping = 0;

// Masks for modifier bits
static const byte kShiftMask = (1 << 0);
static const byte kGreenSquareMask = (1 << 1);
static const byte kOrangeCircleMask = (1 << 2);
static const byte kPeopleMask = (1 << 3);

// Voodoo protocol messages
static const byte kInitMessage[] = { 0x87, 0x02, 0x8C, 0x1F, 0xCC };
static const byte kAwakeMessage[] = { 0x87, 0x02, 0x8C, 0x1B, 0xD0 };

  uint8_t _buffer[8];

  uint8_t _last_modifiers = 0;
  uint8_t _last_key0 = 0;
  uint8_t _last_key1 = 0;

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
Serial1.begin(19200);
Serial1.write(0x87);
Serial1.write(0x02);
Serial1.write(0x8C);
Serial1.write(0x1F);
Serial1.write(0xCC);
}
void awake() {
Serial1.write(0x87);
Serial1.write(0x02);
Serial1.write(0x8C);
Serial1.write(0x1B);
Serial1.write(0xD0);
}

void loop() {
  // put your main code here, to run repeatedly:
  //if (Serial1.available()) {
    //Serial.print(Serial1.read(),HEX);
  //}



 if (Serial1.available() >= 16) {
  
  if(Serial1.peek() != 0xA5 && Serial1.peek() != 0xB4)
  {
    char null = Serial1.read();
    Serial.println(null, HEX);
    //delay(100);
    return;
  }
  
    for (int i = 0; i < 8; i++) {
      _buffer[i] = Serial1.read();
      //if (_buffer[0] != 0xA5 && _buffer[0] != 0xB4) 
      //i--;
    }

    // We expect "status report" packets beginning with 0xA5, but don't know
    // what to do with them -- so we silently discard them.
    if (_buffer[0] == 0xA5) return;

    // We *do not* expect other types of packets.  If we find one, complain
    // to the user.
    if (_buffer[0] != 0xB4) {
      Serial.print("Unexpected packet type: ");
      Serial.println(_buffer[0], HEX);
      delay(100);
      return;
    }
    if (_buffer[1] != 0xC5) {
      Serial.print("Unexpected second byte: ");
      Serial.println(_buffer[1], HEX);
      delay(100);
      return;
    }

    // Check the checksum.
    unsigned char checksum = _buffer[0];
    for (int i = 1; i < 7; i++) checksum += _buffer[i];
    checksum = -checksum;
    if (checksum != _buffer[7]) {
      Serial.println("Checksum failure");
      return;
    }

    // Packet looks good!
    // Dissect the parts we care about:
    byte modifiers = _buffer[3];
    byte key0 = _buffer[4];
    byte key1 = _buffer[5];

    // Check for changes in the modifiers.
    byte modifier_changes = modifiers ^ _last_modifiers;
    if (modifier_changes & kShiftMask) {
      //dispatch(KeyShift, modifiers & kShiftMask);
      Serial.println("shift changed");
    }
    if (modifier_changes & kGreenSquareMask) {
      //dispatch(KeyGreenSquare, modifiers & kGreenSquareMask);
      Serial.println("green changed");
    }
    if (modifier_changes & kOrangeCircleMask) {
      //dispatch(KeyOrangeCircle, modifiers & kOrangeCircleMask);
      Serial.println("orange changed");
    }
    if (modifier_changes & kPeopleMask) {
      //dispatch(KeyPeople, modifiers & kPeopleMask);
      Serial.println("people changed");
    }
    _last_modifiers = modifiers;

    // Check for changes in the other keys
    if (key0 && key0 != _last_key0 && key0 != _last_key1) {
      //dispatch(key0, Down);
      Serial.print(key0);
      Serial.println(" down");
    }
    if (key1 && key1 != _last_key0 && key1 != _last_key1) {
      //dispatch(key1, Down);
      Serial.print(key1);
      Serial.println(" down");
    }
    if (_last_key0 && _last_key0 != key0 && _last_key0 != key1) {
      //dispatch(_last_key0, Up);
      Serial.print(_last_key0);
      Serial.println(" up");
    }
    if (_last_key1 && _last_key1 != key0 && _last_key1 != key1) {
      //dispatch(_last_key1, Up);
      Serial.print(_last_key1);
      Serial.println(" up");
    }
    _last_key0 = key0;
    _last_key1 = key1;
  }


  
    uint32_t time = millis();
  if (time - last_ping > 1000) {
    last_ping = time;
    awake();
  }
}
