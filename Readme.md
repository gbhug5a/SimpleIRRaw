## Simple IR Raw Receive and Send Sketches for ATmega328P Arduinos - Rev 3

### For obscure IR remote profiles, these sketches capture and send in raw format.
### No libraries are required.

### July, 2025 - IR Carrier Frequency Detector Added

This is a short sketch which detects the IR carrier frequency. It is typically
38KHz, but designers may use lower or higher frequencies.  Also included is a
schematic of the circuit needed to receive the carrier.  It uses a typical IR LED
as a poor man's photodiode.  It is much less efficient than a real photodiode,
but good enough for this purpose.  The remote should be fired directly into the
top of the IR LED from about two centimeters away - you may have to experiment.


### Rev 3 - March, 2025

All the code was revised in March, 2025, to deal properly with long values that
exceed 65535 microseconds.  These may occur between repeats.  And the number of
capture entries was increased to 600 over a period of two seconds. The capture
sketch should now capture extended sequences produced by some remote protocols.



In some cases IRremote and other libraries are not able to decode an IR remote's
output to a known profile, and their handling of raw format may not be intuitive.
These sketches capture and send in raw format without using any library.

The send code can be incorporated into a larger sketch after the capture sketch has
been used to capture each key's data. The send sketch fetches the raw data arrays
directly from flash memory using PROGMEM, so they don't use up ram. Any carrier
frequency may be specified, with the befault being 38KHz.

Also included is a third sketch which may be helpful in setting the Adjust variable
in the capture sketch. That's used in case an IR receiver module detects carrier
faster than it detects loss of carrier, or vice versa, by shifting a small amount of
time between the "On" times and the "Off" times.  This sketch transmits carrier with
the On and Off times exactly equal.  If they aren't captured as equal, Adjust can be
modified so they are.

All of these sketches are written for the ATmega328P Arduinos - Uno, Nano and Pro Mini.

In testing, it's best to have the transmitter six feet or more away from the receiver,
and room ambient lighting should be low.  Anything which hampers reception will reduce
the duration of the On periods and increase the duration of the Off periods.  Use the
briefest possible button press to determine the mimimum transmission. Some protocols
require one or more repeats.  The carrier frequency detector is an exception - the
remote must be held very close to the IR LED receiver.

Note: The IR LED drive circuit must have the IR LED *ON* when D10 is high.

