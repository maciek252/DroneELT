################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
PDE_SRCS += \
../FastSerial/examples/FastSerial/FastSerial.pde 

PDE_DEPS += \
./FastSerial/examples/FastSerial/FastSerial.pde.d 


# Each subdirectory must supply rules for building sources it contributes
FastSerial/examples/FastSerial/FastSerial.o: ../FastSerial/examples/FastSerial/FastSerial.pde
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-g++" -c -g -Os -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -MMD -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10605 -DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR     -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/cores/arduino" -I"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/arduino/avr/variants/eightanaloginputs" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '


