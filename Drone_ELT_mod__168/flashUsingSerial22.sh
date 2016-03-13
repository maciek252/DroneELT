#!/bin/sh
/home/maciek/Downloads/kk-multicopter-multiwii/arduino-1.6.7/hardware/tools/avr/bin/avrdude -C/home/maciek/Downloads/kk-multicopter-multiwii/arduino-1.6.7/hardware/tools/avr/etc/avrdude.conf  -v -v -v -v -patmega328p -c arduino -P/dev/ttyUSB0 -b 115200 -Uflash:w:/tmp/build5a19ae446ccdfc101d0bf8d824dbd7b2.tmp/Drone_ELT_mod__168.ino.hex:i 
