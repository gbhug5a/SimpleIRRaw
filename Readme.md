## Simple IR Raw Receive and Send Sketches for ATmega328P Arduinos

### For obscure IR remote profiles, these sketches capture and send in raw format.
### No libraries are required.


In some cases IRremote and other libraries are not able to decode an IR remote's
output to a known profile, and their handling of raw format may not be intuitive.
These sketches capture and send in raw format without using any library.

The send code can be incorporated into a larger sketch after the capture sketch has
been used to capture each key's data. The send sketch fetches the raw data arrays
directly from flash memory using PROGMEM, so they don't use up ram. Any carrier
frequency may be specified.

Also included is a third sketch which may be helpful in setting the Adjust variable
in the capture sketch. That's used in case an IR receiver module detects carrier
faster than it detects loss of carrier, or vice versa, by shifting a small amount of
time between the "On" times and the "Off" times.  This sketch transmits carrier with
the On and Off times exactly equal.  If they aren't captured as equal, Adjust can be
modified until they are.

All of these sketches are written for the ATmega328P Arduinos - Uno, Nano and Pro Mini.

In testing, it's best to have the transmitter six feet or more away from the receiver,
and room ambient lighting should be low.  Anything which hampers reception will reduce
the duration of the On periods and increase the duration of the Off periods.

