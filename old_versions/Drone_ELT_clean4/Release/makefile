################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -f

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include arduino/core/subdir.mk
-include GCS_MAVLink/subdir.mk
-include FastSerial/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
arduino.ar \

ELF += \
Drone_ELT_clean4.elf \

EEP += \
Drone_ELT_clean4.eep \

FLASH_IMAGE += \
Drone_ELT_clean4.hex \


# All Target
all: Drone_ELT_clean4

# Tool invocations
Drone_ELT_clean4: $(FLASH_IMAGE)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-size" -A "/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/Release/Drone_ELT_clean4.elf"
	@echo 'Finished building target: $@'
	@echo ' '

arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-ar" rcs  "/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/Release/arduino.ar"   $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

Drone_ELT_clean4.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-gcc" -Os -Wl,--gc-sections -mmcu=atmega328p  -o "/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/Release/Drone_ELT_clean4.elf"   $(LINK_OBJ) $(AR)  "/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/Release/arduino.ar" "-L/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/Release" -lm
	@echo 'Finished building: $@'
	@echo ' '

Drone_ELT_clean4.eep: $(ELF)
	@echo 'Create eeprom image'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-objcopy" -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0  "/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/Release/Drone_ELT_clean4.elf" "/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/Release/Drone_ELT_clean4.eep"
	@echo 'Finished building: $@'
	@echo ' '

Drone_ELT_clean4.hex: $(EEP)
	@echo 'Create Flash image (ihex format)'
	"/opt-hdd/opt/Downloads/kk-multicopter-multiwii/arduino-1.6.5/hardware/tools/avr/bin/avr-objcopy" -O ihex -R .eeprom  "/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/Release/Drone_ELT_clean4.elf" "/opt-hdd/opt/Downloads/kk-multicopter-multiwii/openLRS/openlrsng-gitsly/DroneELT/Drone_ELT_clean4/Release/Drone_ELT_clean4.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(C_DEPS)$(AR)$(EEP)$(FLASH_IMAGE)$(CC_DEPS)$(AR_OBJ)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(INO_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(ELF)$(CPP_DEPS) Drone_ELT_clean4
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
