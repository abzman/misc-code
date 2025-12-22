#define PRESCALER 1
#define PRESCALER_BITS 0x01
#define CLK 16000000UL    // Default clock speed is 16MHz on Arduino Uno

int setWaveforms( unsigned long freq , int shift ) {
  // This assumes prescaler = 1. For lower freqnecies, use a larger prescaler.
  unsigned long clocks_per_toggle = (CLK / freq) / 2;    // /2 becuase it takes 2 toggles to make a full wave
  ICR1 = clocks_per_toggle;
  unsigned long offset_clocks = (clocks_per_toggle * shift) / 180UL; // Do mult first to save precision
  OCR1B= offset_clocks;
  // Turn on timer now if is was not already on
  // Clock source = clkio/1 (no prescaling)
  // Note: you could use a prescaller here for lower freqnencies
  TCCR1B |= _BV( CS10 ); 
}

void setup() {
  pinMode( 9 , OUTPUT );    // Arduino Pin  9 = OCR1A
  pinMode( 10 , OUTPUT );   // Arduino Pin 10 = OCR1B

  // Both outputs in toggle mode  
  TCCR1A = _BV( COM1A0 ) |_BV( COM1B0 );
  // CTC Waveform Generation Mode
  TCCR1B = _BV( WGM13) | _BV( WGM12);
  OCR1A = 0;    // First output is the base, it always toggles at 0
  setWaveforms( 25000 , 90 ); // Set frequency at 10kHz and set pin 9, 90 degrees out of phase.
} 

void loop() {}
