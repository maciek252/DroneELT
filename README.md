# DroneELT
ELT for drones

DroneELT is an analogue of ELT rescue system for aircrafts. Its core is a Orange LRS module with an antenna and a separate battery, protected by a foam from crash damage, and receiving the MAVLINK data from the drone's flight controller. As soon as the MAVLINK messages stop coming, DroneELT starts to transmit over a predefined PMR frequency the last GPS position encoded in the BCD code.

Format of messages is as follows:

LONG: long, low sound. Corresponds to byte 1.
SHORT: high, short sound. Corresponds to byte 0.
nINFO: a sequence of n very high frequency, very short signals 

1INFO: start of a digit.
2INFO: start and end of a (real) number
3INFO: a dot: end of the integer part, start of the fractional part

GPS position is transmitted in decimal degrees. Default setting is 2 integral digits, 5 fractional digits. So for example

Latitude: 52.2297700
Longitude: 21.0117800

will be
(nI - nINFO, L - LONG, S - SHORT)

 2I 1I S L S L 1I S S L S 3I S S L S 1I S S L S 1I L S S L 1I S L L L 1I S L L L 1I S S S S 1I S S S S 2I
 2I 1I S S L S 1I S S S L 3I S S S S 1I S S S L 1I S S S L 1I S L L L 1I L S S S 1I S S S S 1I S S S S 2I

When MAVLINK messages are transmitted again, DroneELT stops sending the rescue signal, switches the transmitter off and updates the position.
 

COMPILATION OPTIONS:

The code is based on the openlrs-gitsly implementantion. Currently it works for OrangeLRS Rx and Tx modules.

NUMBER_OF_DIGITS_INTEGRAL 
NUMBER_OF_DIGITS_FRACTIONAL
TRANSMIT_ROLL - transmits the roll value instead of the GPS position, for testing when no valid GPS fix is available

PREDEFINED SETTINGS:

-

TODO:

-Android app for decoding the messages
-input directly from GPS (NMEA. UBLOX?)
-sign of the position 
-other data (altitude?)