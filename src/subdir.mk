################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../gzfstream.cpp \
../matching.cpp \
../msArchaicAnalysis.cpp 

OBJS += \
./gzfstream.o \
./matching.o \
./msArchaicAnalysis.o 

CPP_DEPS += \
./gzfstream.d \
./matching.d \
./msArchaicAnalysis.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<" -fopenmp
	@echo 'Finished building: $<'
	@echo ' '


