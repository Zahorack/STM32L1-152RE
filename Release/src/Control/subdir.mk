################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Control/Control.cpp \
../src/Control/ManualMode.cpp \
../src/Control/Packet.cpp 

OBJS += \
./src/Control/Control.o \
./src/Control/ManualMode.o \
./src/Control/Packet.o 

CPP_DEPS += \
./src/Control/Control.d \
./src/Control/ManualMode.d \
./src/Control/Packet.d 


# Each subdirectory must supply rules for building sources it contributes
src/Control/%.o: ../src/Control/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32L1 -DSTM32L152RETx -DNUCLEO_L152RE -DSTM32L152xE -DUSE_HAL_DRIVER -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/Utilities/STM32L1xx_Nucleo" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/HAL_Driver/Inc/Legacy" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/inc" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/CMSIS/device" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/CMSIS/core" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/HAL_Driver/Inc" -O3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


