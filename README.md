# DroneELT
ELT for drones

DroneELT is an analogue of ELT (Emergency Locator Transmitter) rescue system for aircrafts. Its core is a Orange LRS module with an antenna and a separate battery, protected by a foam from crash damage, and receiving the MAVLINK data from the drone's flight controller. As soon as the MAVLINK messages stop coming, DroneELT starts to transmit over a predefined PMR channel (default: PMR 6) the last GPS position encoded in the BCD code.

*WARNING: some features from the description below are currently being implemented. See descriptions of the released versions.*

##Data

MAVlink - standard settings, the same as for MinimOSD:
*RAW_SENSORS=2
*EXTENDED_STATUS=2
*RC_CHANNELS=5
*POSITION=2
*EXTRA1=5
*EXTRA2=2

As of current, no MAVlink is sent (the TX serial line is used for diagnostics)

GPS - NMEA, messages GN*** (including glonass), not tested for GP*** but should 
work


##LEDs 

LEDs are used for signalling the current state of the device with respect to MAVlink and/or NMEA data received:

* __FAST BLINKING__ - waiting for data (every 6 seconds changes between MAVLINK and NMEA led)
* __1 FLASH__ (flash means LED ON, otherwise OFF) - receiving data (MAVLINK or NMEA),
 no valid gps fix
* __2 FLASHES__ - receiving data, valid fix, drone not started yet
* __3_FLASHES__ (flash means LED OFF, otherwise ON) - receiving data, valid fix, drone started (move detected)
* __RED LED ON__: started transmitting alarm
~~* __BOTH LEDS ON__: transmitting alarm, long sound~~
~~* __ONE LED ON__: transmitting alarm, short sound~~


OrangeLRS Tx: red LED - MAVlink, blue LED - GPS NMEA

OrangeLRS Rx: red LED - MAVlink, green LED - GPS NMEA

##Button

Button is present in OrangeLRS Tx. For OrangeLRS Rx, the button pressing effect is achieved by connecting CH3 to GND.

Double clicking the button when there is no valid GPS position: transmits "123.45"

Double clicking the button when there is a valid GPS position but no start detected: transmits the current position

Double clicking after detecting the start: no effect

##Start and triggering alarm criteria

Start criterion:
* valid GPS fix being received
* 5 consecutive positions with distance of 10 meters from the average of the first 5 valid fixes

Triggering alarm criterion:
alarm is triggered if start criterion has been satisfied and 10 seconds have passed after one of the following events:
for average positions from every last 6 seconds:

* 0 or 1 valid fixes
* 2-6 fixes, less than 3 meters between averages from every of these valid samples

When the alarm is started, the position is no longer updated.

##Format of messages is as follows:

Last valid position is transmitted:

* LONG: low sound. Corresponds to byte 1.
* SHORT: high sound. Corresponds to byte 0.
* nINFO: a sequence of n very high frequency, very short signals 

(the notions of long and short are no longer valid, I've tested that the sounds for 0 and 1 should be of equal length, this makes the decoding easier)

* 1INFO: start of a digit.
* 2INFO: start and end of a (real) number
* 3INFO: a dot: end of the integer part, start of the fractional part

GPS position is transmitted in decimal degrees. Default setting is 2 integral digits, 5 fractional digits. So for example

* Latitude: 52.2297700
* Longitude: 21.0117800

will be encoded as: (nI - nINFO, L - LONG, S - SHORT)

* 2I 1I S L S L 1I S S L S 3I S S L S 1I S S L S 1I L S S L 1I S L L L 1I S L L L 1I S S S S 1I S S S S 2I
* 2I 1I S S L S 1I S S S L 3I S S S S 1I S S S L 1I S S S L 1I S L L L 1I L S S S 1I S S S S 1I S S S S 2I

When MAVLINK messages are transmitted again, DroneELT stops sending the rescue signal, switches the transmitter off and updates the position.
 

 
 

##Connection

![](https://github.com/maciek252/DroneELT/blob/master/img/DroneELT.svg.png)

Currently the serial port baudrate is fixed at 57600.

##Compilation options

(not implemented yet)
The code is based on the openlrs-gitsly implementantion. Currently it works for OrangeLRS Tx modules.

NUMBER_OF_DIGITS_INTEGRAL 
NUMBER_OF_DIGITS_FRACTIONAL
TRANSMIT_ROLL - transmits the roll value instead of the GPS position, for testing when no valid GPS fix is available

Predefined settings

-

##Compilation

The main branch is placed in the folder Drone_ELT_mod__168/.
The code compiles using Arduino 1.6.8. There is a makefile using the project Arduino-Makefile, which can be run by executing the script m.sh (the original Makefile is left for ArduinoIDE).

There is an eclipse project, but it currently does not use the arduino plugin. It is placed in folder Drone_ELT_mod2/, but it refers to the source from Drone_ELT_mod/.

##TODO:

* MAVlink or NMEA over I2C, so that two sources of data are available
* Android app for decoding messages
* sign of the position 
* transmitting other data (altitude?)
* choosing the best sample (for example, by looking at HDOP)
* configuration from menu over serial link, similarily as in OrangeLRSng software. Storing the configuration in EEPROM memory.
* transmitting not only the last position, but some number of most recent valid positions, or an average?
* consider switching the alarm off/updating position?
