#!/bin/sh
/home/maciek/Downloads/kk-multicopter-multiwii/arduino-1.6.7/hardware/tools/avr/bin/avrdude -C/home/maciek/Downloads/kk-multicopter-multiwii/arduino-1.6.7/hardware/tools/avr/etc/avrdude.conf  -v -v -v -v -patmega328p -cusbasp -Pusb  -u -U lfuse:w:0xFF:m -U hfuse:w:0xDE:m -U efuse:w:0x05:m
