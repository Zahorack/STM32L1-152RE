################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Util/Bms.cpp \
../src/Util/Communication.cpp \
../src/Util/Logger.cpp \
../src/Util/Timer.cpp \
../src/Util/Trace.cpp 

OBJS += \
./src/Util/Bms.o \
./src/Util/Communication.o \
./src/Util/Logger.o \
./src/Util/Timer.o \
./src/Util/Trace.o 

CPP_DEPS += \
./src/Util/Bms.d \
./src/Util/Communication.d \
./src/Util/Logger.d \
./src/Util/Timer.d \
./src/Util/Trace.d 


# Each subdirectory must supply rules for building sources it contributes
src/Util/%.o: ../src/Util/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32L1 -DSTM32L152RETx -DNUCLEO_L152RE -DSTM32L152xE -DUSE_HAL_DRIVER -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/Utilities/STM32L1xx_Nucleo" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/HAL_Driver/Inc/Legacy" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/inc" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/CMSIS/device" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/CMSIS/core" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/HAL_Driver/Inc" -O3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


