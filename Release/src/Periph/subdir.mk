################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Periph/DigitalOutputPin.cpp \
../src/Periph/Engine.cpp \
../src/Periph/Led.cpp \
../src/Periph/Pwm.cpp \
../src/Periph/Servo.cpp \
../src/Periph/SysTickCounter.cpp \
../src/Periph/Usart.cpp 

OBJS += \
./src/Periph/DigitalOutputPin.o \
./src/Periph/Engine.o \
./src/Periph/Led.o \
./src/Periph/Pwm.o \
./src/Periph/Servo.o \
./src/Periph/SysTickCounter.o \
./src/Periph/Usart.o 

CPP_DEPS += \
./src/Periph/DigitalOutputPin.d \
./src/Periph/Engine.d \
./src/Periph/Led.d \
./src/Periph/Pwm.d \
./src/Periph/Servo.d \
./src/Periph/SysTickCounter.d \
./src/Periph/Usart.d 


# Each subdirectory must supply rules for building sources it contributes
src/Periph/%.o: ../src/Periph/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32L1 -DSTM32L152RETx -DNUCLEO_L152RE -DSTM32L152xE -DUSE_HAL_DRIVER -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/Utilities/STM32L1xx_Nucleo" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/HAL_Driver/Inc/Legacy" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/inc" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/CMSIS/device" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/CMSIS/core" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/HAL_Driver/Inc" -O3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


