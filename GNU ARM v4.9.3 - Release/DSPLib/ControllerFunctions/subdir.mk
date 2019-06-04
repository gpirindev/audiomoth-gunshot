################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DSPLib/ControllerFunctions/arm_pid_init_f32.c \
../DSPLib/ControllerFunctions/arm_pid_init_q15.c \
../DSPLib/ControllerFunctions/arm_pid_init_q31.c \
../DSPLib/ControllerFunctions/arm_pid_reset_f32.c \
../DSPLib/ControllerFunctions/arm_pid_reset_q15.c \
../DSPLib/ControllerFunctions/arm_pid_reset_q31.c \
../DSPLib/ControllerFunctions/arm_sin_cos_f32.c \
../DSPLib/ControllerFunctions/arm_sin_cos_q31.c 

OBJS += \
./DSPLib/ControllerFunctions/arm_pid_init_f32.o \
./DSPLib/ControllerFunctions/arm_pid_init_q15.o \
./DSPLib/ControllerFunctions/arm_pid_init_q31.o \
./DSPLib/ControllerFunctions/arm_pid_reset_f32.o \
./DSPLib/ControllerFunctions/arm_pid_reset_q15.o \
./DSPLib/ControllerFunctions/arm_pid_reset_q31.o \
./DSPLib/ControllerFunctions/arm_sin_cos_f32.o \
./DSPLib/ControllerFunctions/arm_sin_cos_q31.o 

C_DEPS += \
./DSPLib/ControllerFunctions/arm_pid_init_f32.d \
./DSPLib/ControllerFunctions/arm_pid_init_q15.d \
./DSPLib/ControllerFunctions/arm_pid_init_q31.d \
./DSPLib/ControllerFunctions/arm_pid_reset_f32.d \
./DSPLib/ControllerFunctions/arm_pid_reset_q15.d \
./DSPLib/ControllerFunctions/arm_pid_reset_q31.d \
./DSPLib/ControllerFunctions/arm_sin_cos_f32.d \
./DSPLib/ControllerFunctions/arm_sin_cos_q31.d 


# Each subdirectory must supply rules for building sources it contributes
DSPLib/ControllerFunctions/arm_pid_init_f32.o: ../DSPLib/ControllerFunctions/arm_pid_init_f32.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DARM_MATH_CM4=1' '-D__FPU_PRESENT=1' '-DEFM32WG380F256=1' -I"C:/Users/Gregi/gunshot/AudioMoth-Project/fatfs_inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/middleware/usb_gecko/inc" -I"C:/Users/Gregi/gunshot/AudioMoth-Project/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32WG/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\DSP\Include" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\NN\Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=hard -MMD -MP -MF"DSPLib/ControllerFunctions/arm_pid_init_f32.d" -MT"DSPLib/ControllerFunctions/arm_pid_init_f32.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSPLib/ControllerFunctions/arm_pid_init_q15.o: ../DSPLib/ControllerFunctions/arm_pid_init_q15.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DARM_MATH_CM4=1' '-D__FPU_PRESENT=1' '-DEFM32WG380F256=1' -I"C:/Users/Gregi/gunshot/AudioMoth-Project/fatfs_inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/middleware/usb_gecko/inc" -I"C:/Users/Gregi/gunshot/AudioMoth-Project/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32WG/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\DSP\Include" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\NN\Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=hard -MMD -MP -MF"DSPLib/ControllerFunctions/arm_pid_init_q15.d" -MT"DSPLib/ControllerFunctions/arm_pid_init_q15.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSPLib/ControllerFunctions/arm_pid_init_q31.o: ../DSPLib/ControllerFunctions/arm_pid_init_q31.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DARM_MATH_CM4=1' '-D__FPU_PRESENT=1' '-DEFM32WG380F256=1' -I"C:/Users/Gregi/gunshot/AudioMoth-Project/fatfs_inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/middleware/usb_gecko/inc" -I"C:/Users/Gregi/gunshot/AudioMoth-Project/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32WG/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\DSP\Include" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\NN\Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=hard -MMD -MP -MF"DSPLib/ControllerFunctions/arm_pid_init_q31.d" -MT"DSPLib/ControllerFunctions/arm_pid_init_q31.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSPLib/ControllerFunctions/arm_pid_reset_f32.o: ../DSPLib/ControllerFunctions/arm_pid_reset_f32.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DARM_MATH_CM4=1' '-D__FPU_PRESENT=1' '-DEFM32WG380F256=1' -I"C:/Users/Gregi/gunshot/AudioMoth-Project/fatfs_inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/middleware/usb_gecko/inc" -I"C:/Users/Gregi/gunshot/AudioMoth-Project/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32WG/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\DSP\Include" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\NN\Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=hard -MMD -MP -MF"DSPLib/ControllerFunctions/arm_pid_reset_f32.d" -MT"DSPLib/ControllerFunctions/arm_pid_reset_f32.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSPLib/ControllerFunctions/arm_pid_reset_q15.o: ../DSPLib/ControllerFunctions/arm_pid_reset_q15.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DARM_MATH_CM4=1' '-D__FPU_PRESENT=1' '-DEFM32WG380F256=1' -I"C:/Users/Gregi/gunshot/AudioMoth-Project/fatfs_inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/middleware/usb_gecko/inc" -I"C:/Users/Gregi/gunshot/AudioMoth-Project/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32WG/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\DSP\Include" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\NN\Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=hard -MMD -MP -MF"DSPLib/ControllerFunctions/arm_pid_reset_q15.d" -MT"DSPLib/ControllerFunctions/arm_pid_reset_q15.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSPLib/ControllerFunctions/arm_pid_reset_q31.o: ../DSPLib/ControllerFunctions/arm_pid_reset_q31.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DARM_MATH_CM4=1' '-D__FPU_PRESENT=1' '-DEFM32WG380F256=1' -I"C:/Users/Gregi/gunshot/AudioMoth-Project/fatfs_inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/middleware/usb_gecko/inc" -I"C:/Users/Gregi/gunshot/AudioMoth-Project/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32WG/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\DSP\Include" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\NN\Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=hard -MMD -MP -MF"DSPLib/ControllerFunctions/arm_pid_reset_q31.d" -MT"DSPLib/ControllerFunctions/arm_pid_reset_q31.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSPLib/ControllerFunctions/arm_sin_cos_f32.o: ../DSPLib/ControllerFunctions/arm_sin_cos_f32.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DARM_MATH_CM4=1' '-D__FPU_PRESENT=1' '-DEFM32WG380F256=1' -I"C:/Users/Gregi/gunshot/AudioMoth-Project/fatfs_inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/middleware/usb_gecko/inc" -I"C:/Users/Gregi/gunshot/AudioMoth-Project/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32WG/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\DSP\Include" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\NN\Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=hard -MMD -MP -MF"DSPLib/ControllerFunctions/arm_sin_cos_f32.d" -MT"DSPLib/ControllerFunctions/arm_sin_cos_f32.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

DSPLib/ControllerFunctions/arm_sin_cos_q31.o: ../DSPLib/ControllerFunctions/arm_sin_cos_q31.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DARM_MATH_CM4=1' '-D__FPU_PRESENT=1' '-DEFM32WG380F256=1' -I"C:/Users/Gregi/gunshot/AudioMoth-Project/fatfs_inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/middleware/usb_gecko/inc" -I"C:/Users/Gregi/gunshot/AudioMoth-Project/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/Device/SiliconLabs/EFM32WG/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/drivers" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v1.1//hardware/kit/common/bsp" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\DSP\Include" -I"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\NN\Include" -O3 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=hard -MMD -MP -MF"DSPLib/ControllerFunctions/arm_sin_cos_q31.d" -MT"DSPLib/ControllerFunctions/arm_sin_cos_q31.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


