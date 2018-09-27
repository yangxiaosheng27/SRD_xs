################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../src/F2806x_CodeStartBranch.asm \
../src/F2806x_DBGIER.asm \
../src/F2806x_DisInt.asm \
../src/F2806x_usDelay.asm 

C_SRCS += \
../src/F2806x_Adc.c \
../src/F2806x_Comp.c \
../src/F2806x_CpuTimers.c \
../src/F2806x_DefaultIsr.c \
../src/F2806x_Dma.c \
../src/F2806x_ECan.c \
../src/F2806x_ECap.c \
../src/F2806x_EPwm.c \
../src/F2806x_EQep.c \
../src/F2806x_GlobalVariableDefs.c \
../src/F2806x_Gpio.c \
../src/F2806x_HRCap.c \
../src/F2806x_I2C.c \
../src/F2806x_Mcbsp.c \
../src/F2806x_OscComp.c \
../src/F2806x_PieCtrl.c \
../src/F2806x_PieVect.c \
../src/F2806x_Sci.c \
../src/F2806x_Spi.c \
../src/F2806x_SysCtrl.c \
../src/F2806x_TempSensorConv.c 

C_DEPS += \
./src/F2806x_Adc.d \
./src/F2806x_Comp.d \
./src/F2806x_CpuTimers.d \
./src/F2806x_DefaultIsr.d \
./src/F2806x_Dma.d \
./src/F2806x_ECan.d \
./src/F2806x_ECap.d \
./src/F2806x_EPwm.d \
./src/F2806x_EQep.d \
./src/F2806x_GlobalVariableDefs.d \
./src/F2806x_Gpio.d \
./src/F2806x_HRCap.d \
./src/F2806x_I2C.d \
./src/F2806x_Mcbsp.d \
./src/F2806x_OscComp.d \
./src/F2806x_PieCtrl.d \
./src/F2806x_PieVect.d \
./src/F2806x_Sci.d \
./src/F2806x_Spi.d \
./src/F2806x_SysCtrl.d \
./src/F2806x_TempSensorConv.d 

OBJS += \
./src/F2806x_Adc.obj \
./src/F2806x_CodeStartBranch.obj \
./src/F2806x_Comp.obj \
./src/F2806x_CpuTimers.obj \
./src/F2806x_DBGIER.obj \
./src/F2806x_DefaultIsr.obj \
./src/F2806x_DisInt.obj \
./src/F2806x_Dma.obj \
./src/F2806x_ECan.obj \
./src/F2806x_ECap.obj \
./src/F2806x_EPwm.obj \
./src/F2806x_EQep.obj \
./src/F2806x_GlobalVariableDefs.obj \
./src/F2806x_Gpio.obj \
./src/F2806x_HRCap.obj \
./src/F2806x_I2C.obj \
./src/F2806x_Mcbsp.obj \
./src/F2806x_OscComp.obj \
./src/F2806x_PieCtrl.obj \
./src/F2806x_PieVect.obj \
./src/F2806x_Sci.obj \
./src/F2806x_Spi.obj \
./src/F2806x_SysCtrl.obj \
./src/F2806x_TempSensorConv.obj \
./src/F2806x_usDelay.obj 

ASM_DEPS += \
./src/F2806x_CodeStartBranch.d \
./src/F2806x_DBGIER.d \
./src/F2806x_DisInt.d \
./src/F2806x_usDelay.d 

OBJS__QUOTED += \
"src\F2806x_Adc.obj" \
"src\F2806x_CodeStartBranch.obj" \
"src\F2806x_Comp.obj" \
"src\F2806x_CpuTimers.obj" \
"src\F2806x_DBGIER.obj" \
"src\F2806x_DefaultIsr.obj" \
"src\F2806x_DisInt.obj" \
"src\F2806x_Dma.obj" \
"src\F2806x_ECan.obj" \
"src\F2806x_ECap.obj" \
"src\F2806x_EPwm.obj" \
"src\F2806x_EQep.obj" \
"src\F2806x_GlobalVariableDefs.obj" \
"src\F2806x_Gpio.obj" \
"src\F2806x_HRCap.obj" \
"src\F2806x_I2C.obj" \
"src\F2806x_Mcbsp.obj" \
"src\F2806x_OscComp.obj" \
"src\F2806x_PieCtrl.obj" \
"src\F2806x_PieVect.obj" \
"src\F2806x_Sci.obj" \
"src\F2806x_Spi.obj" \
"src\F2806x_SysCtrl.obj" \
"src\F2806x_TempSensorConv.obj" \
"src\F2806x_usDelay.obj" 

C_DEPS__QUOTED += \
"src\F2806x_Adc.d" \
"src\F2806x_Comp.d" \
"src\F2806x_CpuTimers.d" \
"src\F2806x_DefaultIsr.d" \
"src\F2806x_Dma.d" \
"src\F2806x_ECan.d" \
"src\F2806x_ECap.d" \
"src\F2806x_EPwm.d" \
"src\F2806x_EQep.d" \
"src\F2806x_GlobalVariableDefs.d" \
"src\F2806x_Gpio.d" \
"src\F2806x_HRCap.d" \
"src\F2806x_I2C.d" \
"src\F2806x_Mcbsp.d" \
"src\F2806x_OscComp.d" \
"src\F2806x_PieCtrl.d" \
"src\F2806x_PieVect.d" \
"src\F2806x_Sci.d" \
"src\F2806x_Spi.d" \
"src\F2806x_SysCtrl.d" \
"src\F2806x_TempSensorConv.d" 

ASM_DEPS__QUOTED += \
"src\F2806x_CodeStartBranch.d" \
"src\F2806x_DBGIER.d" \
"src\F2806x_DisInt.d" \
"src\F2806x_usDelay.d" 

C_SRCS__QUOTED += \
"../src/F2806x_Adc.c" \
"../src/F2806x_Comp.c" \
"../src/F2806x_CpuTimers.c" \
"../src/F2806x_DefaultIsr.c" \
"../src/F2806x_Dma.c" \
"../src/F2806x_ECan.c" \
"../src/F2806x_ECap.c" \
"../src/F2806x_EPwm.c" \
"../src/F2806x_EQep.c" \
"../src/F2806x_GlobalVariableDefs.c" \
"../src/F2806x_Gpio.c" \
"../src/F2806x_HRCap.c" \
"../src/F2806x_I2C.c" \
"../src/F2806x_Mcbsp.c" \
"../src/F2806x_OscComp.c" \
"../src/F2806x_PieCtrl.c" \
"../src/F2806x_PieVect.c" \
"../src/F2806x_Sci.c" \
"../src/F2806x_Spi.c" \
"../src/F2806x_SysCtrl.c" \
"../src/F2806x_TempSensorConv.c" 

ASM_SRCS__QUOTED += \
"../src/F2806x_CodeStartBranch.asm" \
"../src/F2806x_DBGIER.asm" \
"../src/F2806x_DisInt.asm" \
"../src/F2806x_usDelay.asm" 


