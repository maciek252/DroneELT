################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial/BetterStream.cpp \
/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial/FastSerial.cpp \
/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial/vprintf.cpp 

S_UPPER_SRCS += \
/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial/ftoa_engine.S \
/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial/ultoa_invert.S 

LINK_OBJ += \
./Libraries/FastSerial/BetterStream.cpp.o \
./Libraries/FastSerial/FastSerial.cpp.o \
./Libraries/FastSerial/ftoa_engine.S.o \
./Libraries/FastSerial/ultoa_invert.S.o \
./Libraries/FastSerial/vprintf.cpp.o 

S_UPPER_DEPS += \
./Libraries/FastSerial/ftoa_engine.S.d \
./Libraries/FastSerial/ultoa_invert.S.d 

CPP_DEPS += \
./Libraries/FastSerial/BetterStream.cpp.d \
./Libraries/FastSerial/FastSerial.cpp.d \
./Libraries/FastSerial/vprintf.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/FastSerial/BetterStream.cpp.o: /opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial/BetterStream.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

Libraries/FastSerial/FastSerial.cpp.o: /opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial/FastSerial.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

Libraries/FastSerial/ftoa_engine.S.o: /opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial/ftoa_engine.S
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-gcc" -c -g -x assembler-with-cpp -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

Libraries/FastSerial/ultoa_invert.S.o: /opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial/ultoa_invert.S
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-gcc" -c -g -x assembler-with-cpp -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

Libraries/FastSerial/vprintf.cpp.o: /opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial/vprintf.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/libraries/FastSerial" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '


