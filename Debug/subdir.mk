################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../G308_ImageLoader.o \
../Main.o 

CPP_SRCS += \
../G308_ImageLoader.cpp \
../Main.cpp \
../ShadowMap.cpp \
../TIMER.cpp \
../VECTOR2D.cpp \
../VECTOR3D.cpp \
../scene.cpp 

OBJS += \
./G308_ImageLoader.o \
./Main.o \
./ShadowMap.o \
./TIMER.o \
./VECTOR2D.o \
./VECTOR3D.o \
./scene.o 

CPP_DEPS += \
./G308_ImageLoader.d \
./Main.d \
./ShadowMap.d \
./TIMER.d \
./VECTOR2D.d \
./VECTOR3D.d \
./scene.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


