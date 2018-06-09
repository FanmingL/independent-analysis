################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv5/tools/compiler/c6000_7.4.4

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"../bin/my_sys.obj" \
"../bin/matrix.obj" \
"../bin/led.obj" \
"../bin/iva.obj" \
"../bin/fft.obj" \
"../bin/c6713_emif.obj" \
"../bin/boot_c671x.obj" \
"../bin/MP3Encodercfg_c.obj" \
"../bin/MP3Encodercfg.obj" \
"C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/my_sys.obj" \
"C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/matrix.obj" \
"C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/led.obj" \
"C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/iva.obj" \
"C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/fft.obj" \
"C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/c6713_emif.obj" \
"C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/boot_c671x.obj" \
"C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/MP3Encoder.obj" \
"C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/MP3Encodercfg_c.obj" \
"C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/MP3Encodercfg.obj" \
"../MP3Encoder.cmd" \
"../csl6713.lib" \
"../lib/DEC6713BSL.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include src/subdir_vars.mk
-include lib/subdir_vars.mk
-include bin/subdir_vars.mk
-include subdir_rules.mk
-include src/subdir_rules.mk
-include lib/subdir_rules.mk
-include bin/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: ../bin/MP3Encoder.out

# Tool invocations
../bin/MP3Encoder.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C6000 Linker'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O2 --define="CHIP_6713" --define=c6713 --display_error_number --diag_wrap=off --obj_directory="../bin" --advice:performance=all -z --stack_size=0x400 -m"../bin/MP3Encoder.map" --heap_size=0x400 -i"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/lib" -i"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" -i"C:/ti/bios_5_42_01_09/packages/ti/rtdx/lib/c6000" -i"C:/ti/bios_5_42_01_09/packages/ti/bios/lib" -i"C:/Users/erdou/workspace_v5_5/MP3Encoder" -i"/lib" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="MP3Encoder_linkInfo.xml" --rom_model -o "../bin/MP3Encoder.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_HDRS__QUOTED)$(GEN_MISC_FILES__QUOTED)$(C6000_EXECUTABLE_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(GEN_CMDS__QUOTED) "..\bin\MP3Encoder.out"
	-$(RM) "MP3Encodercfg_c.pp" "src\MP3Encoder.pp" "src\fft.pp" "src\iva.pp" "src\led.pp" "src\matrix.pp" "src\my_sys.pp" 
	-$(RM) "MP3Encodercfg.pp" 
	-$(RM) "C:\Users\erdou\workspace_v5_5\MP3Encoder\bin\MP3Encodercfg.obj" "C:\Users\erdou\workspace_v5_5\MP3Encoder\bin\MP3Encodercfg_c.obj" "C:\Users\erdou\workspace_v5_5\MP3Encoder\bin\MP3Encoder.obj" "C:\Users\erdou\workspace_v5_5\MP3Encoder\bin\boot_c671x.obj" "C:\Users\erdou\workspace_v5_5\MP3Encoder\bin\c6713_emif.obj" "C:\Users\erdou\workspace_v5_5\MP3Encoder\bin\fft.obj" "C:\Users\erdou\workspace_v5_5\MP3Encoder\bin\iva.obj" "C:\Users\erdou\workspace_v5_5\MP3Encoder\bin\led.obj" "C:\Users\erdou\workspace_v5_5\MP3Encoder\bin\matrix.obj" "C:\Users\erdou\workspace_v5_5\MP3Encoder\bin\my_sys.obj" 
	-$(RM) "src\boot_c671x.pp" "src\c6713_emif.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

