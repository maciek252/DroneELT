################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../libraries/FastSerial/BetterStream.cpp \
../libraries/FastSerial/FastSerial.cpp \
../libraries/FastSerial/vprintf.cpp 

S_UPPER_SRCS += \
../libraries/FastSerial/ftoa_engine.S \
../libraries/FastSerial/ultoa_invert.S 

LINK_OBJ += \
./libraries/FastSerial/BetterStream.cpp.o \
./libraries/FastSerial/FastSerial.cpp.o \
./libraries/FastSerial/ftoa_engine.S.o \
./libraries/FastSerial/ultoa_invert.S.o \
./libraries/FastSerial/vprintf.cpp.o 

S_UPPER_DEPS += \
./libraries/FastSerial/ftoa_engine.S.d \
./libraries/FastSerial/ultoa_invert.S.d 

CPP_DEPS += \
./libraries/FastSerial/BetterStream.cpp.d \
./libraries/FastSerial/FastSerial.cpp.d \
./libraries/FastSerial/vprintf.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/FastSerial/BetterStream.cpp.o: ../libraries/FastSerial/BetterStream.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

libraries/FastSerial/FastSerial.cpp.o: ../libraries/FastSerial/FastSerial.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

libraries/FastSerial/ftoa_engine.S.o: ../libraries/FastSerial/ftoa_engine.S
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-gcc" -c -g -x assembler-with-cpp -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

libraries/FastSerial/ultoa_invert.S.o: ../libraries/FastSerial/ultoa_invert.S
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-gcc" -c -g -x assembler-with-cpp -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

libraries/FastSerial/vprintf.cpp.o: ../libraries/FastSerial/vprintf.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '


