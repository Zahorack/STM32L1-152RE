################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/stm32l1xx_it.c \
../src/syscalls.c \
../src/system_stm32l1xx.c 

CPP_SRCS += \
../src/Application.cpp \
../src/main.cpp 

OBJS += \
./src/Application.o \
./src/main.o \
./src/stm32l1xx_it.o \
./src/syscalls.o \
./src/system_stm32l1xx.o 

C_DEPS += \
./src/stm32l1xx_it.d \
./src/syscalls.d \
./src/system_stm32l1xx.d 

CPP_DEPS += \
./src/Application.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32L1 -DSTM32L152RETx -DNUCLEO_L152RE -DDEBUG -DSTM32L152xE -DUSE_HAL_DRIVER -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/Utilities/STM32L1xx_Nucleo" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/HAL_Driver/Inc/Legacy" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/inc" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/CMSIS/device" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/CMSIS/core" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/HAL_Driver/Inc" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32L1 -DSTM32L152RETx -DNUCLEO_L152RE -DDEBUG -DSTM32L152xE -DUSE_HAL_DRIVER -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/Utilities/STM32L1xx_Nucleo" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/HAL_Driver/Inc/Legacy" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/inc" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/CMSIS/device" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/CMSIS/core" -I"C:/Users/Oliver/Documents/workspace/STM32L1-152RE/HAL_Driver/Inc" -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


