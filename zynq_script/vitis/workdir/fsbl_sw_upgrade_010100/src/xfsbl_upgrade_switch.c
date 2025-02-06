#include "xfsbl_upgrade_switch.h"

#include "xfsbl_main.h"
#include "xfsbl_bs.h"
#include "xfsbl_qspi.h"

static int PollBSSwitchCmd(void);
static void WriteCurrentBSVersion(void);
void WriteCurrentSWVersion(void);
static int FlashNewImage(XFsblPs * FsblInstancePtr);
static int PollUpgradeCmd(void);

int XFsbl_BSUpgrade(XFsblPs *FsblInstancePtr) {
	int param;
	u32 MaxPartitions;
	u32 bs_address;
	u32 MultiBootOffset;

	XFsbl_Printf(DEBUG_PRINT_ALWAYS,"Wait command from NXP to switch bitstream or upgrade flash!\n\r");

	MaxPartitions = MAX_BS_COUNT; // On the Zynq slot indexin is from 1 to 8. Index 1 is the bootloader.

	Xil_Out32(UPGRADE_BASE_ADDR+DDR_STATUS_OFFSET, ZYNQ_IS_DDRLESS);

	WriteCurrentBSVersion();
	WriteCurrentSWVersion();

	MultiBootOffset = XFsbl_In32(CSU_CSU_MULTI_BOOT);
	if (MultiBootOffset == MULTIBOOT_OFFSET_FOR_GOLDEN) {
		XFsbl_Printf(DEBUG_PRINT_ALWAYS,"BOOT.bin is located to 0xE000000. This is as the Golden image.\n\r");
		ProtectGoldenImage();
	}

	// Infinitive loop
	while (1) {
		XFsbl_Printf(DEBUG_PRINT_ALWAYS,".");

    	param = PollBSSwitchCmd();
    	if (param) {
			if (param > MaxPartitions) {
				XFsbl_Printf(DEBUG_PRINT_ALWAYS,"Selected bitstream number is too big, max value is %d\n\r", MaxPartitions);
			} else {
				if (param == 1) {
					XFsbl_Printf(DEBUG_PRINT_ALWAYS,"\n\rSwitch to bootloader bitstream!\n\r");

					if (XFsbl_PartitionLoad(FsblInstancePtr, param) == 0) {
						XFsbl_Printf(DEBUG_PRINT_ALWAYS,"Bitstream update success\n\r");
					} else {
						XFsbl_Printf(DEBUG_PRINT_ALWAYS,"Bitstream update failed\n\r");
					}
				} else {
					bs_address = (param - 1) * BS_SLOT_SIZE;
					XFsbl_Printf(DEBUG_PRINT_ALWAYS,"Switch to bitstream on index %d, flash address is 0x%lX\n\r", (param - 1), bs_address);
					if (ConfigureBitstream(FsblInstancePtr, bs_address, BS_FILE_SIZE) == 0) {
						XFsbl_Printf(DEBUG_PRINT_ALWAYS,"Bitstream update success\n\r");
					} else {
						XFsbl_Printf(DEBUG_PRINT_ALWAYS,"Bitstream update failed\n\r");
					}
				}

				Xil_Out32(UPGRADE_BASE_ADDR+DDR_STATUS_OFFSET, ZYNQ_IS_DDRLESS);
				WriteCurrentBSVersion();
				WriteCurrentSWVersion();
			}
    	}

    	param = PollUpgradeCmd();
    	if (param) {
    		FlashNewImage(FsblInstancePtr);
    	}

    	usleep(1000000);
	} //loop

	return 0;
}

int PollUpgradeCmd(void) {
	u32 data;

	data = Xil_In32(UPGRADE_BASE_ADDR+NXP_CTRL_OFFSET);
	//XFsbl_Printf(DEBUG_INFO,"PollUpgradeCmd data = 0x%X\n\r", data);

	if (data == NXP_CTRL_START) {
		XFsbl_Printf(DEBUG_PRINT_ALWAYS,"\n\rNXP ask to upgrade flash, command is %d!\n\r", data);

		Xil_Out32(UPGRADE_BASE_ADDR+NXP_CTRL_OFFSET, 0x0);

		return data;
	}

	return 0;
}

int PollBSSwitchCmd(void) {
	u32 data;

	data = Xil_In32(BRAM_CTRL_STATUS_REG+BS_SWITCH_CTRL_OFFSET);
	if (data != 0) {
		XFsbl_Printf(DEBUG_PRINT_ALWAYS,"\n\rNXP ask to update Bitstream %d!\n\r", data);

		Xil_Out32(BRAM_CTRL_STATUS_REG+BS_SWITCH_CTRL_OFFSET, 0x0);

		return data;
	}

	return 0;
}

int FlashNewImage(XFsblPs * FsblInstancePtr) {
	int block_cnt = 0;
	int buf_sel;
	int FileSize;
	u32 WriteAddr;
	u32 EndAddr;
	u8 *DataPtr;

	WriteAddr = Xil_In32(UPGRADE_BASE_ADDR+IMAGE_ADDRESS_OFFSET);

	FileSize = Xil_In32(UPGRADE_BASE_ADDR+FILESIZE_OFFSET);

	EndAddr = WriteAddr + FileSize;

	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Filesize is %d. Flash write address is 0x%lX, end address is 0x%lX\n\r", FileSize, WriteAddr, EndAddr);
	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Erase flash started!\n\r");

	if (FlashErase(WriteAddr, FileSize)) {
		XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Flash erase failed!\n\r");
		Xil_Out32(UPGRADE_BASE_ADDR+ZYNQ_STATUS_OFFSET, ZYNQ_FLASHING_FAILED);
		return -1;
	}

	Xil_Out32(UPGRADE_BASE_ADDR+ZYNQ_STATUS_OFFSET, ZYNQ_ERASE_DONE);

	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Erase done!\n\r");
	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Data write to flash started!\n\r");

	buf_sel = 1;
	DataPtr = (u8 *)BRAM_BUF_1;

	while (WriteAddr < EndAddr) {
		//XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Buf %d, block cnt %d\n\r", buf_sel, block_cnt);
		do {
			if (Xil_In32(UPGRADE_BASE_ADDR+NXP_BLOCK_CNT_OFFSET) == (block_cnt + 1)) {
				break;
			}
		}while(1);

		block_cnt++;
		Xil_Out32(UPGRADE_BASE_ADDR+ZYNQ_BLOCK_CNT_OFFSET, block_cnt);

		if (FlashWrite(WriteAddr, BRAM_BUF_SIZE, DataPtr)) {
			XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Flash write failed!\n\r");
			Xil_Out32(UPGRADE_BASE_ADDR+ZYNQ_STATUS_OFFSET, ZYNQ_FLASHING_FAILED);
			return-1;
		}

		WriteAddr = WriteAddr + BRAM_BUF_SIZE;
		if (buf_sel == 1) {
			buf_sel = 2;
			DataPtr = (u8 *)BRAM_BUF_2;
		} else {
			buf_sel = 1;
			DataPtr = (u8 *)BRAM_BUF_1;
		}
	}

	Xil_Out32(UPGRADE_BASE_ADDR+ZYNQ_STATUS_OFFSET, ZYNQ_FLASHING_DONE);
	XFsbl_Printf(DEBUG_PRINT_ALWAYS,"Data write done!\n\r");

	return 0;
}

void WriteCurrentSWVersion() {
	u32 SWVersio = SW_VERSION;

	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Current software version is 0x%X \n\r\n\r", SWVersio);
	Xil_Out32((UPGRADE_BASE_ADDR+SW_VERSION_OFFSET), SWVersio);
}

void WriteCurrentBSVersion(void) {
	u32 BSVersio_1;
	u32 BSVersio_2;
	u32 BSVersio_3;

	BSVersio_1 = Xil_In32(BS_VER_REG_FIRST);
	BSVersio_2 = Xil_In32(BS_VER_REG_SECOND);
	BSVersio_3 = Xil_In32(BS_VER_REG_THIRD);
	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Current Bitstream version is 0x%X 0x%X 0x%X\n\r\n\r", BSVersio_1, BSVersio_2, BSVersio_3);

	Xil_Out32((BRAM_CTRL_STATUS_REG+BRAM_BS_VER_OFF_FIRST), BSVersio_1);
	Xil_Out32((BRAM_CTRL_STATUS_REG+BRAM_BS_VER_OFF_SECOND), BSVersio_2);
	Xil_Out32((BRAM_CTRL_STATUS_REG+BRAM_BS_VER_OFF_THIRD), BSVersio_3);
}
