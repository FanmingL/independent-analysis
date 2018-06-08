################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
MP3Encodercfg.cmd: ../MP3Encoder.tcf
	@echo 'Building file: $<'
	@echo 'Invoking: TConf'
	"C:/ti/bios_5_42_01_09/xdctools/tconf" -b -Dconfig.importPath="C:/ti/bios_5_42_01_09/packages;" "$<"
	@echo 'Finished building: $<'
	@echo ' '

MP3Encodercfg.s??: | MP3Encodercfg.cmd
MP3Encodercfg_c.c: | MP3Encodercfg.cmd
MP3Encodercfg.h: | MP3Encodercfg.cmd
MP3Encodercfg.h??: | MP3Encodercfg.cmd
MP3Encoder.cdb: | MP3Encodercfg.cmd

C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/MP3Encodercfg.obj: ./MP3Encodercfg.s?? $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O2 --include_path="C:/Users/erdou/workspace_v5_5/MP3Encoder/include" --include_path="C:/ti/ccsv5/cslc6000/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="C:/Users/erdou/workspace_v5_5/MP3Encoder/Release" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --include_path="C:/ti/xdais_7_21_01_07/packages/ti/xdais" --include_path="/include" --define="CHIP_6713" --define=c6713 --display_error_number --diag_wrap=off --obj_directory="../bin" --advice:performance=all --preproc_with_compile --preproc_dependency="MP3Encodercfg.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

C:/Users/erdou/workspace_v5_5/MP3Encoder/bin/MP3Encodercfg_c.obj: ./MP3Encodercfg_c.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C6000 Compiler'
	"C:/ti/ccsv5/tools/compiler/c6000_7.4.4/bin/cl6x" -mv6700 --abi=coffabi -O2 --include_path="C:/Users/erdou/workspace_v5_5/MP3Encoder/include" --include_path="C:/ti/ccsv5/cslc6000/include" --include_path="C:/ti/ccsv5/tools/compiler/c6000_7.4.4/include" --include_path="C:/Users/erdou/workspace_v5_5/MP3Encoder/Release" --include_path="C:/ti/bios_5_42_01_09/packages/ti/bios/include" --include_path="C:/ti/bios_5_42_01_09/packages/ti/rtdx/include/c6000" --include_path="C:/ti/xdais_7_21_01_07/packages/ti/xdais" --include_path="/include" --define="CHIP_6713" --define=c6713 --display_error_number --diag_wrap=off --obj_directory="../bin" --advice:performance=all --preproc_with_compile --preproc_dependency="MP3Encodercfg_c.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


