/*  AdjustGenerator.ino
 *  for Atmega328P processors (Uno, Nano, ProMini 16MHz)
 *  Circuit must turn on IR LED when D10 is high.
 *  This generates a sequence of 38KHz ~600uS bits to drive an IR LED.
 *  The carrier-on and carrier-off periods are exactly the same.
 *  This is used in conjunction with IRCaptureRaw.ino to set the
 *  value of the Adjust variable so On/Off capture is also equal.
 *  Different IR receiver modules may need different Adjust values.
 *  The transmitter should be at least six feet away, in low ambient light.
 *  An LED driver circuit (transistor) is needed, switched from D10.
 *  Hit Reset to send again.
 *  Thanks to Nick Gammon for most of the code.
 */
const byte LED = 10;  // Timer 1 "B" output: OC1B

// Clock frequency divided by 38 kHz frequency desired
const long timer1_OCR1A_Setting = F_CPU / 38000L;

volatile byte Count = 24;
volatile byte Session = 50;

void setup() {

  TIMSK0 = 0;                                        // Disable Timer0 interrupts (millis)

  pinMode (LED, OUTPUT); digitalWrite(LED,LOW);      // D10 normally output low = IR LED off

  // set up Timer 1 - gives us 38.005 kHz 
  // Fast PWM top at OCR1A
  TCCR1A = bit (WGM10) | bit (WGM11);                // fast PWM
  TCCR1B = bit (WGM12) | bit (WGM13);                // fast PWM, no prescaler
  OCR1A =  timer1_OCR1A_Setting - 1;                 // zero relative  
  OCR1B =  OCR1A /3;                                 // 33% duty cycle
  TCNT1 = OCR1B + 1;                                 // timer count just past OCR1B
  TIMSK1 = bit (OCIE1B);                             // enable interrupt - each cycle
  TCCR1B |= bit (CS10);                              // timer on
}

void loop() {
  while (Session);
  TCCR1B &= ~bit(CS10);                              // stop clock
  TCCR1A &= ~bit(COM1B1);                            // disconnect output
  while(1);
}

ISR(TIMER1_COMPB_vect) {
  Count--;
  if (Count == 0) {
    TCCR1A ^= 0x20;                                  // Connect or disconnect D10 to Timer1 output
    Count = 23;                                      // about 600uS
    Session--;
  }
}
