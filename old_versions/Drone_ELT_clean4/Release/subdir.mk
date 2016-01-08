################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../Drone_ELT_clean4.ino 

CPP_SRCS += \
../.ino.cpp \
../AP_GPS_NMEA.cpp \
../GPS.cpp \
../TinyGPS.cpp \
../storeData.cpp 

LINK_OBJ += \
./.ino.cpp.o \
./AP_GPS_NMEA.cpp.o \
./GPS.cpp.o \
./TinyGPS.cpp.o \
./storeData.cpp.o 

INO_DEPS += \
./Drone_ELT_clean4.ino.d 

CPP_DEPS += \
./.ino.cpp.d \
./AP_GPS_NMEA.cpp.d \
./GPS.cpp.d \
./TinyGPS.cpp.d \
./storeData.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
.ino.cpp.o: ../.ino.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/FastSerial" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/GCS_MAVLink" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

AP_GPS_NMEA.cpp.o: ../AP_GPS_NMEA.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/FastSerial" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/GCS_MAVLink" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

Drone_ELT_clean4.o: ../Drone_ELT_clean4.ino
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/FastSerial" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/GCS_MAVLink" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

GPS.cpp.o: ../GPS.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/FastSerial" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/GCS_MAVLink" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

TinyGPS.cpp.o: ../TinyGPS.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/FastSerial" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/GCS_MAVLink" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

storeData.cpp.o: ../storeData.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/FastSerial" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/GCS_MAVLink" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '


