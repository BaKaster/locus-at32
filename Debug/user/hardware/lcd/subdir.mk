################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../user/hardware/lcd/at32_video_ev_font.c \
../user/hardware/lcd/at32_video_ev_lcd.c 

OBJS += \
./user/hardware/lcd/at32_video_ev_font.o \
./user/hardware/lcd/at32_video_ev_lcd.o 

C_DEPS += \
./user/hardware/lcd/at32_video_ev_font.d \
./user/hardware/lcd/at32_video_ev_lcd.d 


# Each subdirectory must supply rules for building sources it contributes
user/hardware/lcd/%.o: ../user/hardware/lcd/%.c user/hardware/lcd/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections  -g -DAT_START_F403A_V1 -DAT32F403AVGT7 -DUSE_STDPERIPH_DRIVER -I"../include" -I"../include/libraries/drivers/inc" -I"../include/libraries/cmsis/cm4/core_support" -I"../include/libraries/cmsis/cm4/device_support" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


