################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../detail/dummy.cpp \
../detail/glm.cpp 

OBJS += \
./detail/dummy.o \
./detail/glm.o 

CPP_DEPS += \
./detail/dummy.d \
./detail/glm.d 


# Each subdirectory must supply rules for building sources it contributes
detail/%.o: ../detail/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


