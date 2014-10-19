################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Extensions/ARB_multitexture_extension.cpp \
../Extensions/EXT_texture_env_combine_extension.cpp 

OBJS += \
./Extensions/ARB_multitexture_extension.o \
./Extensions/EXT_texture_env_combine_extension.o 

CPP_DEPS += \
./Extensions/ARB_multitexture_extension.d \
./Extensions/EXT_texture_env_combine_extension.d 


# Each subdirectory must supply rules for building sources it contributes
Extensions/%.o: ../Extensions/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


