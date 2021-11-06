################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: surtidor-de-nafta.axf

# Tool invocations
surtidor-de-nafta.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Users\Laudi\Downloads\TPElecDigital3\lpc1769_dispenser\CMSISv2p00_LPC17xx\Debug" -Xlinker -Map="surtidor-de-nafta.map" -Xlinker --cref -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "surtidor-de-nafta_Debug.ld" -o "surtidor-de-nafta.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) surtidor-de-nafta.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "surtidor-de-nafta.axf"; # arm-none-eabi-objcopy -v -O binary "surtidor-de-nafta.axf" "surtidor-de-nafta.bin" ; # checksum -p LPC1769 -d "surtidor-de-nafta.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
