################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include usb/subdir.mk
-include src/subdir.mk
-include fatfs/subdir.mk
-include emlib/subdir.mk
-include drivers/subdir.mk
-include NN/subdir.mk
-include CMSIS/EFM32WG/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: AudioMoth-Project.axf

# Tool invocations
AudioMoth-Project.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "AudioMoth-Project.ld" -L"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\Lib\GCC" -L"C:\Users\Gregi\gunshot\CMSIS_5-5.4.0\CMSIS\Lib\ARM" -Xlinker --gc-sections -Xlinker -Map="AudioMoth-Project.map" -mfpu=fpv4-sp-d16 -mfloat-abi=hard --specs=nano.specs -o AudioMoth-Project.axf "./usb/em_usbd.o" "./usb/em_usbdch9.o" "./usb/em_usbdep.o" "./usb/em_usbdint.o" "./usb/em_usbh.o" "./usb/em_usbhal.o" "./usb/em_usbhep.o" "./usb/em_usbhint.o" "./usb/em_usbtimer.o" "./src/audioMoth.o" "./src/main.o" "./fatfs/diskio.o" "./emlib/em_acmp.o" "./emlib/em_adc.o" "./emlib/em_aes.o" "./emlib/em_assert.o" "./emlib/em_burtc.o" "./emlib/em_can.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_cryotimer.o" "./emlib/em_csen.o" "./emlib/em_dac.o" "./emlib/em_dbg.o" "./emlib/em_dma.o" "./emlib/em_ebi.o" "./emlib/em_emu.o" "./emlib/em_gpcrc.o" "./emlib/em_gpio.o" "./emlib/em_i2c.o" "./emlib/em_idac.o" "./emlib/em_ldma.o" "./emlib/em_lesense.o" "./emlib/em_letimer.o" "./emlib/em_leuart.o" "./emlib/em_mpu.o" "./emlib/em_msc.o" "./emlib/em_opamp.o" "./emlib/em_pcnt.o" "./emlib/em_prs.o" "./emlib/em_qspi.o" "./emlib/em_rmu.o" "./emlib/em_rtc.o" "./emlib/em_rtcc.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./emlib/em_vcmp.o" "./emlib/em_vdac.o" "./emlib/em_wdog.o" "./drivers/dmactrl.o" "./drivers/microsd.o" "./CMSIS/EFM32WG/startup_efm32wg.o" "./CMSIS/EFM32WG/system_efm32wg.o" "./fatfs/audioMoth_ff.o" "./src/predictor.o" -larm_cortexM4lf_math -lm "./src/main2.o" "./NN/arm_convolve_1x1_HWC_q7_fast_nonsquare.o" "./NN/arm_convolve_HWC_q15_fast_nonsquare.o" "./NN/arm_convolve_HWC_q7_basic_nonsquare.o" "./NN/arm_convolve_HWC_q7_fast_nonsquare.o" "./NN/arm_depthwise_separable_conv_HWC_q7_nonsquare.o" "./NN/arm_fully_connected_q15.o" "./NN/arm_fully_connected_q7.o" "./NN/arm_nn_activations_q15.o" "./NN/arm_nn_activations_q7.o" "./NN/arm_nntables.o" "./NN/arm_relu_q15.o" "./NN/arm_relu_q7.o" "./src/test.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: AudioMoth-Project.hex'
	arm-none-eabi-objcopy -O ihex "AudioMoth-Project.axf" "AudioMoth-Project.hex"
	@echo ' '

	@echo 'Building bin file: AudioMoth-Project.bin'
	arm-none-eabi-objcopy -O binary "AudioMoth-Project.axf" "AudioMoth-Project.bin"
	@echo ' '

	@echo 'Building s37 file: AudioMoth-Project.s37'
	arm-none-eabi-objcopy -O srec "AudioMoth-Project.axf" "AudioMoth-Project.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "AudioMoth-Project.axf"
	@echo ' '

	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) AudioMoth-Project.axf
	-@echo ' '

post-build:
	-@echo 'Generate listing'
	-arm-none-eabi-objdump -d -t -S AudioMoth-Project.axf > AudioMoth-Project.lst
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
