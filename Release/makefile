################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./src/F2806x_Adc.obj" \
"./src/F2806x_CodeStartBranch.obj" \
"./src/F2806x_Comp.obj" \
"./src/F2806x_CpuTimers.obj" \
"./src/F2806x_DBGIER.obj" \
"./src/F2806x_DefaultIsr.obj" \
"./src/F2806x_DisInt.obj" \
"./src/F2806x_Dma.obj" \
"./src/F2806x_ECan.obj" \
"./src/F2806x_ECap.obj" \
"./src/F2806x_EPwm.obj" \
"./src/F2806x_EQep.obj" \
"./src/F2806x_GlobalVariableDefs.obj" \
"./src/F2806x_Gpio.obj" \
"./src/F2806x_HRCap.obj" \
"./src/F2806x_I2C.obj" \
"./src/F2806x_Mcbsp.obj" \
"./src/F2806x_OscComp.obj" \
"./src/F2806x_PieCtrl.obj" \
"./src/F2806x_PieVect.obj" \
"./src/F2806x_Sci.obj" \
"./src/F2806x_Spi.obj" \
"./src/F2806x_SysCtrl.obj" \
"./src/F2806x_TempSensorConv.obj" \
"./src/F2806x_usDelay.obj" \
"./user/light_matrix.obj" \
"./user/main.obj" \
"./user/srm_adapt.obj" \
"./user/srm_adc.obj" \
"./user/srm_control.obj" \
"./user/srm_error.obj" \
"./user/srm_gpio.obj" \
"./user/srm_pwm.obj" \
"./user/srm_qep.obj" \
"./user/srm_sensor.obj" \
"./user/srm_timer.obj" \
"../cmd/F28069M.cmd" \
"../cmd/F2806x_Headers_nonBIOS.cmd" \
"../lib/2806x_BootROM_API_TABLE_Symbols.lib" \
"../lib/2806x_BootROM_API_TABLE_Symbols_fpu32.lib" \
"../lib/SFO_TI_Build_V6b.lib" \
"../lib/SFO_TI_Build_V6b_FPU.lib" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include cmd/subdir_vars.mk
-include lib/subdir_vars.mk
-include src/subdir_vars.mk
-include user/subdir_vars.mk
-include cmd/subdir_rules.mk
-include lib/subdir_rules.mk
-include src/subdir_rules.mk
-include user/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
SRM_Release.out \

EXE_OUTPUTS__QUOTED += \
"SRM_Release.out" \

BIN_OUTPUTS += \
SRM_Release.hex \

BIN_OUTPUTS__QUOTED += \
"SRM_Release.hex" \


# All Target
all: SRM_Release.out

# Tool invocations
SRM_Release.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"D:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla0 --float_support=fpu32 --vcu_support=vcu0 -O2 --diag_warning=225 --diag_wrap=off --display_error_number -z -m"SRM_Release.map" --heap_size=0x400 --stack_size=0x300 --warn_sections -i"D:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.1.LTS/lib" -i"D:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="SRM_Release_linkInfo.xml" --rom_model -o "SRM_Release.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

SRM_Release.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: C2000 Hex Utility'
	"D:/ti/ccsv8/tools/compiler/ti-cgt-c2000_18.1.1.LTS/bin/hex2000"  -o "SRM_Release.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "src\F2806x_Adc.obj" "src\F2806x_CodeStartBranch.obj" "src\F2806x_Comp.obj" "src\F2806x_CpuTimers.obj" "src\F2806x_DBGIER.obj" "src\F2806x_DefaultIsr.obj" "src\F2806x_DisInt.obj" "src\F2806x_Dma.obj" "src\F2806x_ECan.obj" "src\F2806x_ECap.obj" "src\F2806x_EPwm.obj" "src\F2806x_EQep.obj" "src\F2806x_GlobalVariableDefs.obj" "src\F2806x_Gpio.obj" "src\F2806x_HRCap.obj" "src\F2806x_I2C.obj" "src\F2806x_Mcbsp.obj" "src\F2806x_OscComp.obj" "src\F2806x_PieCtrl.obj" "src\F2806x_PieVect.obj" "src\F2806x_Sci.obj" "src\F2806x_Spi.obj" "src\F2806x_SysCtrl.obj" "src\F2806x_TempSensorConv.obj" "src\F2806x_usDelay.obj" "user\light_matrix.obj" "user\main.obj" "user\srm_adapt.obj" "user\srm_adc.obj" "user\srm_control.obj" "user\srm_error.obj" "user\srm_gpio.obj" "user\srm_pwm.obj" "user\srm_qep.obj" "user\srm_sensor.obj" "user\srm_timer.obj" 
	-$(RM) "src\F2806x_Adc.d" "src\F2806x_Comp.d" "src\F2806x_CpuTimers.d" "src\F2806x_DefaultIsr.d" "src\F2806x_Dma.d" "src\F2806x_ECan.d" "src\F2806x_ECap.d" "src\F2806x_EPwm.d" "src\F2806x_EQep.d" "src\F2806x_GlobalVariableDefs.d" "src\F2806x_Gpio.d" "src\F2806x_HRCap.d" "src\F2806x_I2C.d" "src\F2806x_Mcbsp.d" "src\F2806x_OscComp.d" "src\F2806x_PieCtrl.d" "src\F2806x_PieVect.d" "src\F2806x_Sci.d" "src\F2806x_Spi.d" "src\F2806x_SysCtrl.d" "src\F2806x_TempSensorConv.d" "user\light_matrix.d" "user\main.d" "user\srm_adapt.d" "user\srm_adc.d" "user\srm_control.d" "user\srm_error.d" "user\srm_gpio.d" "user\srm_pwm.d" "user\srm_qep.d" "user\srm_sensor.d" "user\srm_timer.d" 
	-$(RM) "src\F2806x_CodeStartBranch.d" "src\F2806x_DBGIER.d" "src\F2806x_DisInt.d" "src\F2806x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

