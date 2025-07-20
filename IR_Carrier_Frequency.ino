// IR_Carrier_Frequency.ino - Uno, Nano, Pro Mini(16MHz) (ATMega328P)
// capture incoming raw IR carrier
// total cycle time - time between falling edges
// 26.32us cycles = 38KHz carrier
// Connect special input circuit to D8

unsigned int total, durations[52];
volatile unsigned int Timer;
volatile unsigned int Capture;
unsigned int prevTimer;
int i, count;
bool endit = false;
float avgCycle;

void setup() {
  pinMode(8,INPUT);
  Serial.begin(57600);
  cli();
  TCCR1A = 0;                                     // set up Timer1
  TCCR1B = 0;
  TCCR1C = 0;
  TCNT1  = 0;                                     // clear Timer1 count
  sei();
}

void loop() {
  cli();
  TIMSK0 = 0;                                     // Disable Timer0 interrupts (millis)
  TIFR1  = 0xFF;                                  // clear flags register
  TCCR1B = 0b00000001;                            // falling edge capture, timer1 on, prescale /1
  TIFR1  = 0xFF;                                  // clear flags
  TIMSK1 = 0b00100000;                            // enable capture interrupt
  TIFR1  = 0xFF;                                  // clear flags
  TCCR1A = 0b00000000;                            // Normal mode, no output, WGM #0
  sei();

  count = 0;
  endit = false;
  Capture = false;

  while (Capture == false);                       // Wait for signal to go LOW
  prevTimer = Timer;

  while ((count < 52) && (endit == false)) {
    durations[count] = Timer - prevTimer;         // vs previous timer count
    count++;
    prevTimer = Timer;
    Capture = false;                              // reset Capture
    while (Capture == false) {                    // Waiting for next Capture
      if ((TCNT1 - prevTimer) > 2000) {           // if too long, stop
        endit = true;
        break;
      }
    }
  }

  TCCR1B &= 0xFE;                                 // stop Timer1 clock
  TIMSK1 = 0b000000000;                           // disable capture interrupt
  TIMSK0 = 1;                                     // resume millis interrupt

  total = 0;
  Serial.println("---");
  Serial.println("Clock cycles at 16MHz:");
  for (i = 2; i < count; i++) {                   // first two cycles can be a bit wonky
    total += durations[i];
    Serial.print(durations[i]);
    Serial.print(" ");
  }
  Serial.print("\nSum of cycle times = ");
  Serial.println(total);
  Serial.print("Number of cycles = ");
  Serial.println (count-2);
  Serial.print("Average cycle time in microseconds = ");
  avgCycle =  ((float)total / ((count-2) * 16));
  Serial.println(avgCycle);
  Serial.print("Frequency in KHz = ");
  Serial.println(1000/avgCycle);  
  delay(1000);                                    // Small pause between readings
}

ISR(TIMER1_CAPT_vect) {                           // Timer1 capture ISR 
  Timer = ICR1;
  Capture = true;
}
