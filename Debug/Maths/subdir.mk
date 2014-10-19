################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Maths/COLOR.cpp \
../Maths/MATRIX4X4.cpp \
../Maths/PLANE.cpp \
../Maths/VECTOR2D.cpp \
../Maths/VECTOR3D.cpp \
../Maths/VECTOR4D.cpp 

OBJS += \
./Maths/COLOR.o \
./Maths/MATRIX4X4.o \
./Maths/PLANE.o \
./Maths/VECTOR2D.o \
./Maths/VECTOR3D.o \
./Maths/VECTOR4D.o 

CPP_DEPS += \
./Maths/COLOR.d \
./Maths/MATRIX4X4.d \
./Maths/PLANE.d \
./Maths/VECTOR2D.d \
./Maths/VECTOR3D.d \
./Maths/VECTOR4D.d 


# Each subdirectory must supply rules for building sources it contributes
Maths/%.o: ../Maths/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


