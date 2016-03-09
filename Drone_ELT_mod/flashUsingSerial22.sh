#!/bin/sh
/home/maciek/Downloads/kk-multicopter-multiwii/arduino-1.6.7/hardware/tools/avr/bin/avrdude -C/home/maciek/Downloads/kk-multicopter-multiwii/arduino-1.6.7/hardware/tools/avr/etc/avrdude.conf  -v -v -v -v -patmega328p -c arduino -P/dev/ttyUSB0 -b 115200 -Uflash:w:/tmp/build7384815869939952033.tmp/Drone_ELT_mod.cpp.hex:i 
