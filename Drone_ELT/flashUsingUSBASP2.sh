#!/bin/sh
avrdude -C/home/maciek/Downloads/kk-multicopter-multiwii/arduino-1.5.5/hardware/tools/avrdude.conf -v -v -v -v -patmega328p -cusbasp -Pusb -Uflash:w:build-pro5v328/Drone_ELT.hex:i 
