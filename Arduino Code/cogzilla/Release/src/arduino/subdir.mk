################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/arduino/HardwareSerial.cpp \
../src/arduino/Print.cpp \
../src/arduino/WMath.cpp \
../src/arduino/Wire.cpp 

C_SRCS += \
../src/arduino/WInterrupts.c \
../src/arduino/pins_arduino.c \
../src/arduino/twi.c \
../src/arduino/wiring.c \
../src/arduino/wiring_analog.c \
../src/arduino/wiring_digital.c \
../src/arduino/wiring_pulse.c \
../src/arduino/wiring_shift.c 

OBJS += \
./src/arduino/HardwareSerial.o \
./src/arduino/Print.o \
./src/arduino/WInterrupts.o \
./src/arduino/WMath.o \
./src/arduino/Wire.o \
./src/arduino/pins_arduino.o \
./src/arduino/twi.o \
./src/arduino/wiring.o \
./src/arduino/wiring_analog.o \
./src/arduino/wiring_digital.o \
./src/arduino/wiring_pulse.o \
./src/arduino/wiring_shift.o 

C_DEPS += \
./src/arduino/WInterrupts.d \
./src/arduino/pins_arduino.d \
./src/arduino/twi.d \
./src/arduino/wiring.d \
./src/arduino/wiring_analog.d \
./src/arduino/wiring_digital.d \
./src/arduino/wiring_pulse.d \
./src/arduino/wiring_shift.d 

CPP_DEPS += \
./src/arduino/HardwareSerial.d \
./src/arduino/Print.d \
./src/arduino/WMath.d \
./src/arduino/Wire.d 


# Each subdirectory must supply rules for building sources it contributes
src/arduino/%.o: ../src/arduino/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega1280 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/arduino/%.o: ../src/arduino/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -mmcu=atmega1280 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


