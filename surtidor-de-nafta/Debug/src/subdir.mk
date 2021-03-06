################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Uart.c \
../src/Uart2.c \
../src/Utils.c \
../src/adc.c \
../src/cr_startup_lpc175x_6x.c \
../src/crp.c \
../src/dma_uart.c \
../src/main.c 

OBJS += \
./src/Uart.o \
./src/Uart2.o \
./src/Utils.o \
./src/adc.o \
./src/cr_startup_lpc175x_6x.o \
./src/crp.o \
./src/dma_uart.o \
./src/main.o 

C_DEPS += \
./src/Uart.d \
./src/Uart2.d \
./src/Utils.d \
./src/adc.d \
./src/cr_startup_lpc175x_6x.d \
./src/crp.d \
./src/dma_uart.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__LPC17XX__ -D__REDLIB__ -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m3 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


