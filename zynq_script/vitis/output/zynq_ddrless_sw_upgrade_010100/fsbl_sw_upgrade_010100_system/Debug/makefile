#
# This file was created by Xilinx Vitis tool.
#


#
# tools
#

RM = rm -f
RMDIR = rm -rf



#
# variables
#

SDCARD_GEN = sdcard_gen
GENERATE_BIF = ::scw::generate_bif

XPFM_PATH = /home/bhashini/Desktop/Zynq_Selftest_new/2025_01_30/vitis_test_with_original_xsa/design_1_wrapper_010200_1/export/design_1_wrapper_010200_1/design_1_wrapper_010200_1.xpfm
SYS_CONFIG = design_1_wrapper_010200_1
DOMAINS = domain_psu_cortexa53_0
BIF_PATH = /home/bhashini/Desktop/Bootloader_Scripting/Scripting/zynq_script/vitis/output/zynq_ddrless_sw_upgrade_010100/fsbl_sw_upgrade_010100_system/Debug/system.bif

BITSTREAM_PATH = /home/bhashini/Desktop/Bootloader_Scripting/Scripting/zynq_script/vitis/output/zynq_ddrless_sw_upgrade_010100/fsbl_sw_upgrade_010100/_ide/bitstream/design_1_wrapper.bit
SD_FILES = --elf /home/bhashini/Desktop/Bootloader_Scripting/Scripting/zynq_script/vitis/output/zynq_ddrless_sw_upgrade_010100/fsbl_sw_upgrade_010100/Debug/fsbl_sw_upgrade_010100.elf,psu_cortexa53_0

GENERATE_BIF_XSCT_CMD = ${GENERATE_BIF} -xpfm ${XPFM_PATH} -domains ${DOMAINS} -bifpath ${BIF_PATH}

#
# primary build targets
#

all: package

package:
	-@echo Generating bif file for the system project
	-@echo Executing command \'${GENERATE_BIF_XSCT_CMD}\' on XSCT
	-@echo "connect -u TCP:localhost:37211; tcf send_command $$::xsdb::curchan xsdb eval s es [list \"${GENERATE_BIF_XSCT_CMD}\"]" | xsct
	${SDCARD_GEN} --xpfm ${XPFM_PATH} --sys_config ${SYS_CONFIG} --bif ${BIF_PATH} --bitstream ${BITSTREAM_PATH} ${SD_FILES}

clean:
	$(RMDIR) ./_sds
	${RMDIR} package
	${RM} system.bif

.PHONY: all package clean


