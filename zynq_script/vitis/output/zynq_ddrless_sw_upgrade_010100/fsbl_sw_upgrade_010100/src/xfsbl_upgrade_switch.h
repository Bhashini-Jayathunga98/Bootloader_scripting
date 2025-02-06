#ifndef XFSBL_BS_SWITCH_H
#define XFSBL_BS_SWITCH_H

#include "xfsbl_main.h"

/* Version format is major.minor.patch, two byte for each. Two first bytes not used. */
#define SW_VERSION					0x00010100

#define BRAM_BUF_SIZE					4096

#define BRAM_BUF_1					0xA0021000
#define BRAM_BUF_2					0xA0022000
#define BRAM_CTRL_STATUS_REG		0xA0020000

/* 1 = buffer is full, 0 = empty or Kintex is already flashed buffer. */
#define	BUF_1_STATUS						0x01
#define	BUF_2_STATUS						0x02

#define	NXP_CTRL_START						0x02

#define	ZYNQ_FLASHING_DONE				0x01
#define	ZYNQ_FLASHING_FAILED			0x02
#define	ZYNQ_ERASE_DONE					0x03

#define	ZYNQ_IS_DDRLESS					0x01

#define BS_SWITCH_CTRL_OFFSET			0x00
#define BRAM_BS_VER_OFF_FIRST			0x10
#define BRAM_BS_VER_OFF_SECOND			0x14
#define BRAM_BS_VER_OFF_THIRD			0x18

#define UPGRADE_BASE_ADDR				BRAM_CTRL_STATUS_REG+0x100

#define	NXP_CTRL_OFFSET						0x00
#define	BUF_STATUS_OFFSET					0x04
#define	ZYNQ_STATUS_OFFSET					0x08
#define	ZYNQ_BLOCK_CNT_OFFSET				0x0C
#define	NXP_BLOCK_CNT_OFFSET				0x10
#define	NXP_DATA_OFFSET						0x14

#define FILESIZE_OFFSET						0x18
#define DDR_STATUS_OFFSET					0x1C
#define IMAGE_ADDRESS_OFFSET				0x20

#define SW_VERSION_OFFSET					0x24

#define BS_SLOT_SIZE					0x2000000
#define MAX_BS_COUNT					7

/* 0x1C00*32kB = 0xE000000 */
#define MULTIBOOT_OFFSET_FOR_GOLDEN		0x1C00

#define BS_FILE_SIZE					23580908

#define BS_VER_REG_FIRST				0xA0010040
#define BS_VER_REG_SECOND				0xA0010044
#define BS_VER_REG_THIRD				0xA0010048


int XFsbl_BSUpgrade(XFsblPs * FsblInstancePtr);

#endif
