################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Analog.cpp \
../src/ArduinoLink.cpp \
../src/GPS.cpp \
../src/I2C.cpp \
../src/main.cpp 

OBJS += \
./src/Analog.o \
./src/ArduinoLink.o \
./src/GPS.o \
./src/I2C.o \
./src/main.o 

CPP_DEPS += \
./src/Analog.d \
./src/ArduinoLink.d \
./src/GPS.d \
./src/I2C.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega1280 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


