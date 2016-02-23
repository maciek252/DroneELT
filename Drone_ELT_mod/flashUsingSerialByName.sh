#!/bin/sh
/home/maciek/Downloads/kk-multicopter-multiwii/arduino-1.6.7/hardware/tools/avr/bin/avrdude -C/home/maciek/Downloads/kk-multicopter-multiwii/arduino-1.6.7/hardware/tools/avr/etc/avrdude.conf  -v -v -v -v -patmega328p -c arduino -P/dev/ttyUSB2 -b 115200 -Uflash:w:$1:i 
