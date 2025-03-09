/*
   IRSendRaw.ino   Rev 3
   Sends raw IR data on D10
   For ATmega328P - Uno, Nano, Pro Mini
   No libraries needed
   Sends automatically on boot.  Hit reset to send again.
   Timing entries fetched directly from flash - PROGMEM
   Circuit must turn on IR LED when D10 is high
*/

#ifndef __AVR_ATmega328P__
#error - Board must use ATmega328P microcontroller
#endif

const unsigned int frequency = 38;                   // frequency in KHz (typically 38)
const byte irLED = 10;                               // Timer 1 "B" output: OC1B
volatile unsigned int irCount;
volatile unsigned int newirCount;
volatile unsigned int irK;
unsigned int oldirK;
byte oldTIMSK0;
unsigned long countDiv, halfDiv;

// raw definition arrays must be global
// sample keypress in NEC protocol

const unsigned long Key[] PROGMEM = {                // values loaded as longs
  9000, 4500, 562, 562, 562, 562, 562, 562,          // example of NEC protocol
  562, 1688, 562, 562, 562, 562, 562, 562,
  562, 562, 562, 562, 562, 1688, 562, 1688,
  562, 562, 562, 1688, 562, 562, 562, 562,
  562, 562, 562, 1688, 562, 1688, 562, 1688,
  562, 562, 562, 562, 562, 562, 562, 562,
  562, 1688, 562, 562, 562, 562, 562, 562,
  562, 1688, 562, 1688, 562, 1688, 562, 1688,
  562, 562, 562
};

void setup() {

  pinMode (irLED, OUTPUT);                           // D10 normally output low = IR LED off
  digitalWrite(irLED, LOW);
  pinMode (LED_BUILTIN, OUTPUT);                     // indicator LED
  digitalWrite(LED_BUILTIN, LOW);
  TCCR1A = bit (WGM10) | bit (WGM11);                // fast PWM, normal port
  TCCR1B = bit (WGM12) | bit (WGM13);                // fast PWM, timer stopped
  TIMSK1 = bit (OCIE1B);                             // enable interrupt - each cycle
}

void loop() {

  delay(2000);
  sendRaw (Key , sizeof(Key) / 4, frequency);        // Send an array of raw data, KHz
  while (1);                                         // endless loop
}

void sendRaw(const unsigned long buf[], unsigned int len, unsigned int hz) {

  oldTIMSK0 = TIMSK0;                                // disable millis interrupt
  TIMSK0 = 0;
  digitalWrite(LED_BUILTIN, HIGH);                   // indicator LED on
  irK = 0;
  oldirK = 0;
  countDiv = (1024000 + (hz / 2)) / hz;              // rounded number of hz cycles * 1024
  halfDiv = (countDiv + 1) / 2;                      // half of countDiv
  irCount = Calculate(pgm_read_dword_near(buf)) + 1;    // timer count for first value of array
  newirCount = Calculate(pgm_read_dword_near(buf + 1)); //    and second value
  OCR1A = (((F_CPU / 1000) + hz / 2) / hz) - 1;      // TOP, zero relative
  OCR1B = OCR1A / 3;                                 // 33% duty cycle
  TCNT1 = OCR1A / 2;                                 // timer count just past OCR1B
  TCCR1A |= bit (COM1B1);                            // clear output on B match
  TCCR1B |= bit (CS10);                              // fast PWM, no prescaler, timer on

  while (irK < len) {
    while (oldirK == irK);                           // wait for interrupt
    if (irK < (len - 1)) {
      newirCount = Calculate (pgm_read_dword_near(buf + (irK + 1)));
    }
    oldirK = irK;
  }
  TCCR1B &= ~bit(CS10);                              // stop clock
  TCCR1A &= ~bit(COM1B1);                            // disconnect output
  TIMSK0 = oldTIMSK0;                                // re-enable millis
  digitalWrite(LED_BUILTIN, LOW);                    // indicator LED off
}

unsigned int Calculate (unsigned long buf) {         // calculate number of timer cycles
  return ((buf << 10) + halfDiv) / countDiv;
}

ISR(TIMER1_COMPB_vect) {                             // interrupt when output cleared at OCR1B
  irCount--;
  if (irCount == 0) {
    TCCR1A ^= bit (COM1B1);                          // Connect or disconnect D10 to Timer1 output
    irK++;
    irCount = newirCount;                            // get next value
  }
}
