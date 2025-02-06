/******************************************************************************
* Copyright (c) 2015 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
*
* @file xfsbl_qspi.c
*
* This is the file which contains qspi related code for the FSBL.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  kc   10/21/13 Initial release
* 2.00  sg   12/03/15 Added GQSPI driver support
*                     32Bit boot mode support
* 3.0   bv   12/02/16 Made compliance to MISRAC 2012 guidelines
*       ds   01/03/17 Add support for Micron QSPI 2G part
* 4.0   tjs  10/16/18 Added support for QPI mode in Macronix flash parts.
*       sk   03/13/19 Added dual parallel support and QPI support for 24bit
*                     boot mode for Macronix flash parts.
*
* </pre>
*
* @note
*
******************************************************************************/
/***************************** Include Files *********************************/
#include "xfsbl_hw.h"
#include "xparameters.h"
#include "xil_cache.h"

#ifdef XFSBL_QSPI
#include "xqspipsu.h"
#include "xfsbl_qspi.h"
#include "xqspipsu_flash_config.h"

/************************** Constant Definitions *****************************/
/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define QSPI_DEVICE_ID		XPAR_XQSPIPSU_0_DEVICE_ID

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
static u32 FlashReadID(XQspiPsu *QspiPsuPtr);
static u32 MacronixEnable4B(XQspiPsu *QspiPsuPtr);
static u32 MacronixEnableQPIMode(XQspiPsu *QspiPsuPtr, int Enable);
static u32 GetRealAddr(XQspiPsu *QspiPsuPtr, u32 Address);
static u32 MicronEnable4B(XQspiPsu *QspiPsuPtr);

/************************** Variable Definitions *****************************/
static XQspiPsu QspiPsuInstance;
static u32 QspiFlashSize=0U;
static u32 QspiFlashMake=0U;
static u32 ReadCommand=0U;
static u32 EraseCommand=0U;
static u32 WriteCommand=0U;
static XQspiPsu_Msg FlashMsg[5];
static u8 IssiIdFlag=0U;

static u8 TxBfrPtr __attribute__ ((aligned(32)));
static u8 ReadBuffer[10] __attribute__ ((aligned(32)));
static u8 WriteBuffer[10] __attribute__ ((aligned(32)));
static u32 MacronixFlash = 0U;

static u32 FCTIndex;
static u8 FSRFlag;
static u8 StatusCmd;
/******************************************************************************
*
* This function reads serial FLASH ID connected to the SPI interface.
* It then deduces the make and size of the flash and obtains the
* connection mode to point to corresponding parameters in the flash
* configuration table. The flash driver will function based on this and
* it presently supports Micron and Spansion - 128, 256 and 512Mbit and
* Winbond 128Mbit
*
* @param	none
*
* @return	XST_SUCCESS if read id, otherwise XST_FAILURE.
*
* @note		None.
*
******************************************************************************/
static u32 FlashReadID(XQspiPsu *QspiPsuPtr)
{
	s32 Status;
	u32 UStatus;
	u32 ReadId = 0;

	/*
	 * Read ID
	 */
	TxBfrPtr = READ_ID_CMD;
	FlashMsg[0].TxBfrPtr = &TxBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	FlashMsg[1].TxBfrPtr = NULL;
	FlashMsg[1].RxBfrPtr = ReadBuffer;
	FlashMsg[1].ByteCount = 4;
	FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, &FlashMsg[0], 2);
	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_FAILURE;
		goto END;
	}

	XFsbl_Printf(DEBUG_GENERAL, "FlashID=0x%x 0x%x 0x%x\n\r", ReadBuffer[0],
					ReadBuffer[1], ReadBuffer[2]);

	ReadId = ((ReadBuffer[0] << 16) | (ReadBuffer[1] << 8) | ReadBuffer[2]);

	Status = CalculateFCTIndex(ReadId, &FCTIndex);
	if (Status != XST_SUCCESS) {
		XFsbl_Printf(DEBUG_GENERAL, "CalculateFCTIndex failed!\n\r");
		return XST_FAILURE;
	}

	/*
	 * Deduce flash make
	 */
	MacronixFlash = 0U;
	if (ReadBuffer[0] == MICRON_ID) {
		QspiFlashMake = MICRON_ID;
		XFsbl_Printf(DEBUG_INFO, "MICRON ");
	} else if(ReadBuffer[0] == SPANSION_ID) {
		QspiFlashMake = SPANSION_ID;
		XFsbl_Printf(DEBUG_INFO, "SPANSION ");
	} else if(ReadBuffer[0] == WINBOND_ID) {
		QspiFlashMake = WINBOND_ID;
		XFsbl_Printf(DEBUG_INFO, "WINBOND ");
	} else if(ReadBuffer[0] == MACRONIX_ID) {
		QspiFlashMake = MACRONIX_ID;
		XFsbl_Printf(DEBUG_INFO, "MACRONIX ");
		MacronixFlash = 1U;
	} else if(ReadBuffer[0] == ISSI_ID) {
		QspiFlashMake = ISSI_ID;
		XFsbl_Printf(DEBUG_INFO, "ISSI ");
	} else {
		UStatus = XFSBL_ERROR_UNSUPPORTED_QSPI;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_UNSUPPORTED_QSPI\r\n");
		goto END;
	}

	/*
	 * Deduce flash Size
	 */
	if (ReadBuffer[2] == FLASH_SIZE_ID_8M) {
		QspiFlashSize = FLASH_SIZE_8M;
		XFsbl_Printf(DEBUG_INFO, "8M Bits\r\n");
	} else if (ReadBuffer[2] == FLASH_SIZE_ID_16M) {
		QspiFlashSize = FLASH_SIZE_16M;
		XFsbl_Printf(DEBUG_INFO, "16M Bits\r\n");
	} else if (ReadBuffer[2] == FLASH_SIZE_ID_32M) {
		QspiFlashSize = FLASH_SIZE_32M;
		XFsbl_Printf(DEBUG_INFO, "32M Bits\r\n");
	} else if (ReadBuffer[2] == FLASH_SIZE_ID_64M) {
		QspiFlashSize = FLASH_SIZE_64M;
		XFsbl_Printf(DEBUG_INFO, "64M Bits\r\n");
	} else if (ReadBuffer[2] == FLASH_SIZE_ID_128M) {
		QspiFlashSize = FLASH_SIZE_128M;
		XFsbl_Printf(DEBUG_INFO, "128M Bits\r\n");
	} else if ((ReadBuffer[2] == FLASH_SIZE_ID_256M)
			|| (ReadBuffer[2] == MACRONIX_FLASH_1_8_V_MX25_ID_256)) {
		QspiFlashSize = FLASH_SIZE_256M;
		XFsbl_Printf(DEBUG_INFO, "256M Bits\r\n");
	} else if ((ReadBuffer[2] == FLASH_SIZE_ID_512M)
			|| (ReadBuffer[2] == MACRONIX_FLASH_SIZE_ID_512M)
			|| (ReadBuffer[2] == MACRONIX_FLASH_1_8_V_MX66_ID_512)) {
		QspiFlashSize = FLASH_SIZE_512M;
		XFsbl_Printf(DEBUG_INFO, "512M Bits\r\n");
	} else if ((ReadBuffer[2] == FLASH_SIZE_ID_1G)
			|| (ReadBuffer[2] == MACRONIX_FLASH_SIZE_ID_1G)
			|| (ReadBuffer[2] == MACRONIX_FLASH_1_8_V_SIZE_ID_1G)) {
		QspiFlashSize = FLASH_SIZE_1G;
		XFsbl_Printf(DEBUG_INFO, "1G Bits\r\n");
	} else if ((ReadBuffer[2] == FLASH_SIZE_ID_2G)
			|| (ReadBuffer[2] == MACRONIX_FLASH_SIZE_ID_2G)
			|| (ReadBuffer[2] == MACRONIX_FLASH_1_8_V_SIZE_ID_2G)) {
                QspiFlashSize = FLASH_SIZE_2G;
                XFsbl_Printf(DEBUG_INFO, "2G Bits\r\n");
	}else {
		UStatus = XFSBL_ERROR_UNSUPPORTED_QSPI;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_UNSUPPORTED_QSPI\r\n");
		goto END;
	}
	/* Enable ID flag for ISSI 128M Qspi to enable
	 * DUAL_READ_CMD_24BIT ReadCommand
	 */
	if((QspiFlashMake==ISSI_ID) && (QspiFlashSize==FLASH_SIZE_128M))
	{
		IssiIdFlag=1;
	}
	UStatus = XFSBL_SUCCESS;

	/* Status cmd - SR or FSR selection */
	if ((Flash_Config_Table[FCTIndex].NumDie > 1) &&
			(QspiFlashMake == MICRON_ID)) {
		StatusCmd = READ_FLAG_STATUS_CMD;
		FSRFlag = 1;
	} else {
		StatusCmd = READ_STATUS_CMD;
		FSRFlag = 0;
	}

END:
	return UStatus;
}

/*****************************************************************************/
/**
 * This function is used to initialize the qspi controller and driver
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
u32 XFsbl_Qspi24Init(u32 DeviceFlags)
{
	XQspiPsu_Config *QspiConfig;
	s32 Status;
	u32 UStatus;
	u32 QspiMode;



	/**
	 * Initialize the QSPI driver so that it's ready to use
	 */
	QspiConfig =  XQspiPsu_LookupConfig(QSPI_DEVICE_ID);
	if (NULL == QspiConfig) {
		UStatus = XFSBL_ERROR_QSPI_INIT;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_INIT\r\n");
		goto END;
	}

	Status =  XQspiPsu_CfgInitialize(&QspiPsuInstance, QspiConfig,
			QspiConfig->BaseAddress);
	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_ERROR_QSPI_INIT;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_INIT\r\n");
		goto END;
	}

	/*
	 * Set Manual Start
	 */
	Status = XQspiPsu_SetOptions(&QspiPsuInstance, XQSPIPSU_MANUAL_START_OPTION);

	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_ERROR_QSPI_MANUAL_START;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_MANUAL_START\r\n");
		goto END;
	}
	/*
	 * Set the pre-scaler for QSPI clock
	 */
	Status = XQspiPsu_SetClkPrescaler(&QspiPsuInstance, XQSPIPSU_CLK_PRESCALE_8);

	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_ERROR_QSPI_PRESCALER_CLK;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_PRESCALER_CLK\r\n");
		goto END;
	}
	XQspiPsu_SelectFlash(&QspiPsuInstance,
		XQSPIPSU_SELECT_FLASH_CS_LOWER, XQSPIPSU_SELECT_FLASH_BUS_LOWER);

	/*
	 * Configure the qspi in linear mode if running in XIP
	 * TBD: XIP Support
	 */

	switch ((u32)(XPAR_PSU_QSPI_0_QSPI_MODE)) {

		case XQSPIPSU_CONNECTION_MODE_SINGLE:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is in single flash connection\r\n");
		} break;

		case XQSPIPSU_CONNECTION_MODE_PARALLEL:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is in Dual Parallel connection\r\n");
		} break;

		case XQSPIPSU_CONNECTION_MODE_STACKED:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is in Dual Stack connection\r\n");
		}break;

		default:
		{
			Status = XFSBL_ERROR_INVALID_QSPI_CONNECTION;
			XFsbl_Printf(DEBUG_GENERAL,
					"XFSBL_ERROR_INVALID_QSPI_CONNECTION\r\n");
		}
		break;

	}
	if(Status != XFSBL_SUCCESS) {
		UStatus = (u32)Status;
		goto END;
	}

	switch (QspiPsuInstance.Config.BusWidth) {

		case XFSBL_QSPI_BUSWIDTH_ONE:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is using 1 bit bus\r\n");
			ReadCommand = FAST_READ_CMD_24BIT;
		} break;

		case XFSBL_QSPI_BUSWIDTH_TWO:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is using 2 bit bus\r\n");
			ReadCommand = DUAL_READ_CMD_24BIT;
		} break;

		case XFSBL_QSPI_BUSWIDTH_FOUR:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is using 4 bit bus\r\n");
			ReadCommand = QUAD_READ_CMD_24BIT;
		}break;

	}

	/**
	 * Read Flash ID and extract Manufacture and Size information
	 */
	UStatus = FlashReadID(&QspiPsuInstance);
	if (UStatus != XFSBL_SUCCESS) {
		goto END;
	}
	/**
	 *  add code for 1x, 2x and 4x
	 *
	 */
	if(IssiIdFlag==1U) {
		ReadCommand = DUAL_READ_CMD_24BIT;
	}

	if ((MacronixFlash == 1U) &&
			(QspiPsuInstance.Config.BusWidth == XFSBL_QSPI_BUSWIDTH_FOUR)) {
		ReadCommand = QUAD_READ_CMD_24BIT2;
	}
	/**
	 * add code: For a Stacked connection, read second Flash ID
	 */
	QspiMode=QspiPsuInstance.Config.ConnectionMode;
	if ((QspiMode ==(XQSPIPSU_CONNECTION_MODE_PARALLEL)) ||
			(QspiMode ==(XQSPIPSU_CONNECTION_MODE_STACKED) )) {
		QspiFlashSize = 2 * QspiFlashSize;
	}

END:
	return UStatus;
}



/******************************************************************************
*
* This functions translates the address based on the type of interconnection.
* In case of stacked, this function asserts the corresponding slave select.
*
* @param	Address which is to be accessed
*
* @return	QspiAddr is the actual flash address to be accessed - for single
* 			it is unchanged; for stacked, the lower flash size is subtracted;
* 			for parallel the address is divided by 2.
*
* @note		None.
*
*
******************************************************************************/
static u32 XFsbl_GetQspiAddr(u32 Address )
{
	u32 RealAddr;

	switch(QspiPsuInstance.Config.ConnectionMode) {

		case XQSPIPSU_CONNECTION_MODE_SINGLE:
			XQspiPsu_SelectFlash(&QspiPsuInstance,
					XQSPIPSU_SELECT_FLASH_CS_LOWER,
					XQSPIPSU_SELECT_FLASH_BUS_LOWER);
			RealAddr = Address;
			break;

		case XQSPIPSU_CONNECTION_MODE_STACKED:
			/*
			 * Select lower or upper Flash based on sector address
			 */
			if(Address >= (QspiFlashSize/2U)) {
				XQspiPsu_SelectFlash(&QspiPsuInstance,
						XQSPIPSU_SELECT_FLASH_CS_UPPER,
						XQSPIPSU_SELECT_FLASH_BUS_LOWER);
				/*
				 * Subtract first flash size when accessing second flash
				 */
				RealAddr = Address - (QspiFlashSize/2U);
			}else{
				/*
				 * Set selection to L_PAGE
				 */
				XQspiPsu_SelectFlash(&QspiPsuInstance,
						XQSPIPSU_SELECT_FLASH_CS_LOWER,
						XQSPIPSU_SELECT_FLASH_BUS_LOWER);
				RealAddr = Address;
			}
			break;

		case XQSPIPSU_CONNECTION_MODE_PARALLEL:
			/*
			 * The effective address in each flash is the actual
			 * address / 2
			 */
			XQspiPsu_SelectFlash(&QspiPsuInstance,
			XQSPIPSU_SELECT_FLASH_CS_BOTH, XQSPIPSU_SELECT_FLASH_BUS_BOTH);
			RealAddr = Address / 2U;
			break;

		default:
			/*
			 * We should never reach here as error will be triggered during
			 * QSPI Init for invalid connection mode. Hence, assign a value (0)
			 * to RealAddr, to avoid warning.
			 */
			RealAddr = 0U;
			break;
	}

	return(RealAddr);
}

/******************************************************************************
*
* This functions selects the current bank
*
* @param	QspiPtr is a pointer to the QSPI driver component to use.
* @param	Pointer to the write buffer which contains data to be transmitted
* @param	BankSel is the bank to be selected in the flash device(s).
*
* @return	XST_SUCCESS if bank selected, otherwise XST_FAILURE.
*
* @note		None.
*
*
******************************************************************************/
static u32 SendBankSelect(u32 BankSel)
{
	s32 Status;
	u32 UStatus;

	/*
	 * bank select commands for Micron and Spansion are different
	 * Macronix bank select is same as Micron
	 */
	if ((QspiFlashMake == MICRON_ID) || (QspiFlashMake == MACRONIX_ID))	{
		/*
		 * For micron command WREN should be sent first
		 * except for some specific feature set
		 */
		TxBfrPtr = WRITE_ENABLE_CMD;
		FlashMsg[0].TxBfrPtr = &TxBfrPtr;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 1);
		if (Status != XFSBL_SUCCESS) {
			UStatus = XFSBL_ERROR_QSPI_READ;
			XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
			goto END;
		}

		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		WriteBuffer[COMMAND_OFFSET]   = EXTADD_REG_WR_CMD;
		WriteBuffer[ADDRESS_1_OFFSET] = (u8)BankSel;

		FlashMsg[0].TxBfrPtr = WriteBuffer;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 2;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 1);
		if (Status != XFSBL_SUCCESS) {
			UStatus = XFSBL_ERROR_QSPI_READ;
			XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
			goto END;
		}
	}

	if (QspiFlashMake == SPANSION_ID) {
		/*
		 * Send the Extended address register write command
		 * written, no receive buffer required
		 */
		WriteBuffer[COMMAND_OFFSET]   = BANK_REG_WR_CMD;
		WriteBuffer[ADDRESS_1_OFFSET] = (u8)BankSel;

		FlashMsg[0].TxBfrPtr = WriteBuffer;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 2;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 1);
		if (Status != XFSBL_SUCCESS) {
			UStatus = XFSBL_ERROR_QSPI_READ;
			XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
			goto END;
		}
	}

	/*
	 * For testing - Read bank to verify
	 */
	if ((QspiFlashMake == MICRON_ID) || (QspiFlashMake == MACRONIX_ID)) {
		/*
		 * Extended address register read command
		 */

		WriteBuffer[COMMAND_OFFSET]   = EXTADD_REG_RD_CMD;
		FlashMsg[0].TxBfrPtr = WriteBuffer;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = ReadBuffer;
		FlashMsg[1].ByteCount = 1;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

		Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 2);
		if (Status != XFSBL_SUCCESS) {
			UStatus = XFSBL_ERROR_QSPI_READ;
			XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
			goto END;
		}
	}

	if (QspiFlashMake == SPANSION_ID) {
		/*
		 * Bank register read command
		 */
		WriteBuffer[COMMAND_OFFSET]   = BANK_REG_RD_CMD;
		FlashMsg[0].TxBfrPtr = WriteBuffer;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = NULL;
		FlashMsg[1].RxBfrPtr = ReadBuffer;
		FlashMsg[1].ByteCount = 1;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;

		Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 2);
		if (Status != XFSBL_SUCCESS) {
			UStatus = XFSBL_ERROR_QSPI_READ;
			XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
			goto END;
		}

		if (ReadBuffer[0] != BankSel) {
			XFsbl_Printf(DEBUG_INFO, "Bank Select %d != Register Read %d\n\r", BankSel,
				ReadBuffer[0]);
			UStatus = XFSBL_ERROR_QSPI_READ;
			XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
			goto END;
		}
	}
	UStatus = XFSBL_SUCCESS;
	/* Winbond can be added here */
END:
	return UStatus;
}

/*****************************************************************************/
/**
 * This function is used to copy the data from QSPI flash to destination
 * address
 *
 * @param SrcAddress is the address of the QSPI flash where copy should
 * start from
 *
 * @param DestAddress is the address of the destination where it
 * should copy to
 *
 * @param Length Length of the bytes to be copied
 *
 * @return
 * 		- XFSBL_SUCCESS for successful copy
 * 		- errors as mentioned in xfsbl_error.h
 *
 *****************************************************************************/
u32 XFsbl_Qspi24Copy(u32 SrcAddress, PTRSIZE DestAddress, u32 Length)
{
	u32 Status;
	u32 QspiAddr, OrigAddr;
	u32 BankSel;
	u32 RemainingBytes;
	u32 TransferBytes;
	u32 DiscardByteCnt;
	u8 BankSwitchFlag=0U;
	u32 BankSize;
	u32 BankMask;
	s32 SStatus;

	XFsbl_Printf(DEBUG_INFO,"QSPI Reading Src 0x%0lx, Dest %0lx, Length %0lx\r\n",
			SrcAddress, DestAddress, Length);

	/**
	 * Check the read length with Qspi flash size
	 */
	if ((SrcAddress + Length) > QspiFlashSize)
	{
		Status = XFSBL_ERROR_QSPI_LENGTH;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_LENGTH\r\n");
		goto END;
	}

	/* Multiply bank size & mask in case of Dual Parallel */
	if (QspiPsuInstance.Config.ConnectionMode ==
	    XQSPIPSU_CONNECTION_MODE_PARALLEL){
		BankSize =  SINGLEBANKSIZE * 2U;
		BankMask =  SINGLEBANKMASK * 2U;
	}
	else
	{
		BankSize = SINGLEBANKSIZE;
		BankMask = SINGLEBANKMASK;
	}

	/**
	 * Update no of bytes to be copied
	 */
	RemainingBytes = Length;

	while(RemainingBytes > 0U) {

		if (RemainingBytes > DMA_DATA_TRAN_SIZE)
		{
			TransferBytes = DMA_DATA_TRAN_SIZE;
		} else {
			TransferBytes = RemainingBytes;
		}

		/**
		 * Translate address based on type of connection
		 * If stacked assert the slave select based on address
		 */
		QspiAddr = XFsbl_GetQspiAddr((u32 )SrcAddress);

		/**
		 * Multiply address by 2 in case of Dual Parallel
		 * This address is used to calculate the bank crossing
		 * condition
		 */
		if (QspiPsuInstance.Config.ConnectionMode ==
		    XQSPIPSU_CONNECTION_MODE_PARALLEL){
			OrigAddr = QspiAddr * 2U;
		} else {
			OrigAddr = QspiAddr;
		}

		/**
		 * Select bank
		 * check logic for DualQspi
		 */
		if(QspiFlashSize > BankSize) {
			BankSel = QspiAddr/BANKSIZE;
			Status = SendBankSelect(BankSel);
			if (Status != XFSBL_SUCCESS) {
				Status = XFSBL_ERROR_QSPI_READ;
				XFsbl_Printf(DEBUG_GENERAL,
					"XFSBL_ERROR_QSPI_READ\r\n");
				goto END;
			}
		}


		/**
		 * If data to be read spans beyond the current bank, then
		 * calculate Transfer Bytes in current bank. Else
		 * transfer bytes are same
		 */
		if ((OrigAddr & BankMask) != ((OrigAddr + TransferBytes) & BankMask)) {
			TransferBytes = (OrigAddr & BankMask) + BankSize - OrigAddr;
		}

		XFsbl_Printf(DEBUG_INFO,".");
				XFsbl_Printf(DEBUG_DETAILED,
					"QSPI Read Src 0x%0lx, Dest %0lx, Length %0lx\r\n",
						QspiAddr, DestAddress, TransferBytes);

		/**
		 * Setup the read command with the specified address and data for the
		 * Flash
		 */
		if ((MacronixFlash == 1U) &&
				(QspiPsuInstance.Config.BusWidth == XFSBL_QSPI_BUSWIDTH_FOUR)) {

			/* Enable QPI mode */
			SStatus = MacronixEnableQPIMode(&QspiPsuInstance, ENABLE_QPI);
			if (SStatus != XFSBL_SUCCESS) {
				Status = XFSBL_ERROR_QSPI_READ;
				XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
				goto END;
			}

			/*Command*/
			WriteBuffer[COMMAND_OFFSET]   = (u8)ReadCommand;
			DiscardByteCnt = 1;
			/*MACRONIX is in QPI MODE 4-4-4*/
			FlashMsg[0].TxBfrPtr = WriteBuffer;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = DiscardByteCnt;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
			/*Address*/
			WriteBuffer[ADDRESS_1_OFFSET] = (u8)((QspiAddr & 0xFF0000U) >> 16);
			WriteBuffer[ADDRESS_2_OFFSET] = (u8)((QspiAddr & 0xFF00U) >> 8);
			WriteBuffer[ADDRESS_3_OFFSET] = (u8)(QspiAddr & 0xFFU);
			DiscardByteCnt = 3;

			FlashMsg[1].TxBfrPtr = &WriteBuffer[ADDRESS_1_OFFSET];
			FlashMsg[1].RxBfrPtr = NULL;
			FlashMsg[1].ByteCount = DiscardByteCnt;
			FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
			FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_TX;

			/*Dummy*/
			/*Default Dummy is 6 when QPI READ MODE(ECH)*/
			FlashMsg[2].TxBfrPtr = NULL;
			FlashMsg[2].RxBfrPtr = NULL;
			FlashMsg[2].ByteCount = DUMMY_CLOCKS_MACRONIX;
			FlashMsg[2].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
			FlashMsg[2].Flags = 0U;

			/*Data*/
			FlashMsg[3].TxBfrPtr = NULL;
			FlashMsg[3].RxBfrPtr = (u8 *)DestAddress;
			FlashMsg[3].ByteCount = TransferBytes;
			FlashMsg[3].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
			FlashMsg[3].Flags = XQSPIPSU_MSG_FLAG_RX;
			if(QspiPsuInstance.Config.ConnectionMode ==
					XQSPIPSU_CONNECTION_MODE_PARALLEL){
				FlashMsg[3].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
			}

			SStatus = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 4);
			if (SStatus != XFSBL_SUCCESS) {
				Status = XFSBL_ERROR_QSPI_READ;
				XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
				goto END;
			}

			/* Disable QPI mode */
			SStatus = MacronixEnableQPIMode(&QspiPsuInstance, DISABLE_QPI);
			if (SStatus != XFSBL_SUCCESS) {
				Status = XFSBL_ERROR_QSPI_READ;
				XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
				goto END;
			}
		} else {
			WriteBuffer[COMMAND_OFFSET]   = (u8)ReadCommand;
			WriteBuffer[ADDRESS_1_OFFSET] = (u8)((QspiAddr & 0xFF0000U) >> 16);
			WriteBuffer[ADDRESS_2_OFFSET] = (u8)((QspiAddr & 0xFF00U) >> 8);
			WriteBuffer[ADDRESS_3_OFFSET] = (u8)(QspiAddr & 0xFFU);
			DiscardByteCnt = 4;

			FlashMsg[0].TxBfrPtr = WriteBuffer;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = DiscardByteCnt;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

			/* It is recommended to have a separate entry for dummy */
			if ((ReadCommand == FAST_READ_CMD_24BIT) || (ReadCommand == DUAL_READ_CMD_24BIT) ||
				(ReadCommand == QUAD_READ_CMD_24BIT)) {
				/* Update Dummy cycles as per flash specs for QUAD IO */

				/*
				 * It is recommended that Bus width value during dummy
				 * phase should be same as data phase
				 */
				if (ReadCommand == FAST_READ_CMD_24BIT) {
					FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
				}

				if (ReadCommand == DUAL_READ_CMD_24BIT) {
					FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_DUALSPI;
				}

				if (ReadCommand == QUAD_READ_CMD_24BIT) {
					FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
				}

				FlashMsg[1].TxBfrPtr = NULL;
				FlashMsg[1].RxBfrPtr = NULL;
				FlashMsg[1].ByteCount = DUMMY_CLOCKS;
				FlashMsg[1].Flags = 0U;
			}

			if (ReadCommand == FAST_READ_CMD_24BIT) {
				FlashMsg[2].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			}

			if (ReadCommand == DUAL_READ_CMD_24BIT) {
				FlashMsg[2].BusWidth = XQSPIPSU_SELECT_MODE_DUALSPI;
			}

			if (ReadCommand == QUAD_READ_CMD_24BIT) {
				FlashMsg[2].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
			}

			FlashMsg[2].TxBfrPtr = NULL;
			FlashMsg[2].RxBfrPtr = (u8 *)DestAddress;
			FlashMsg[2].ByteCount = TransferBytes;
			FlashMsg[2].Flags = XQSPIPSU_MSG_FLAG_RX;

			if(QspiPsuInstance.Config.ConnectionMode ==
					XQSPIPSU_CONNECTION_MODE_PARALLEL){
				FlashMsg[2].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
			}

			/**
			 * Send the read command to the Flash to read the specified number
			 * of bytes from the Flash, send the read command and address and
			 * receive the specified number of bytes of data in the data buffer
			 */
			SStatus = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 3);
			if (SStatus != XFSBL_SUCCESS) {
				Status = XFSBL_ERROR_QSPI_READ;
				XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
				goto END;
			}
		}

		/**
		 * Update the variables
		 */
		RemainingBytes -= TransferBytes;
		DestAddress += TransferBytes;
		SrcAddress += TransferBytes;
	}

	if(BankSwitchFlag == 1U)
	{
		/*
		 * Reset Bank selection to zero
		 */
		Status = SendBankSelect(0);
		if (Status != XFSBL_SUCCESS) {
			Status = XFSBL_ERROR_QSPI_READ;
			XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
			goto END;
		}
	}
	else
	{
		Status = XFSBL_SUCCESS;
	}

END:
	return (u32)Status;
}

/*****************************************************************************/
/**
 * This function is used to release the Qspi settings
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
u32 XFsbl_Qspi24Release(void)
{
	u32 Status = XFSBL_SUCCESS;

	return Status;
}


/*****************************************************************************/
/**
 * This function is used to initialize the qspi controller and driver
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
u32 XFsbl_Qspi32Init(u32 DeviceFlags)
{
	XQspiPsu_Config *QspiConfig;
	s32 Status;
	u32 QspiMode;
	u32 UStatus;



	/**
	 * Initialize the QSPI driver so that it's ready to use
	 */
	QspiConfig =  XQspiPsu_LookupConfig(QSPI_DEVICE_ID);
	if (NULL == QspiConfig) {
		UStatus = XFSBL_ERROR_QSPI_INIT;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_INIT\r\n");
		goto END;
	}

	Status =  XQspiPsu_CfgInitialize(&QspiPsuInstance, QspiConfig,
			QspiConfig->BaseAddress);
	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_ERROR_QSPI_INIT;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_INIT\r\n");
		goto END;
	}

	/*
	 * Set Manual Start
	 */
	Status = XQspiPsu_SetOptions(&QspiPsuInstance, XQSPIPSU_MANUAL_START_OPTION);

	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_ERROR_QSPI_MANUAL_START;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_MANUAL_START\r\n");
		goto END;
	}
	/*
	 * Set the pre-scaler for QSPI clock
	 */
	Status = XQspiPsu_SetClkPrescaler(&QspiPsuInstance, XQSPIPSU_CLK_PRESCALE_8);

	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_ERROR_QSPI_PRESCALER_CLK;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_PRESCALER_CLK\r\n");
		goto END;
	}
	XQspiPsu_SelectFlash(&QspiPsuInstance,
		XQSPIPSU_SELECT_FLASH_CS_LOWER, XQSPIPSU_SELECT_FLASH_BUS_LOWER);

	/*
	 * Configure the qspi in linear mode if running in XIP
	 * TBD
	 */

	switch ((u32)XPAR_PSU_QSPI_0_QSPI_MODE) {

		case XQSPIPSU_CONNECTION_MODE_SINGLE:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is in single flash connection\r\n");
		} break;

		case XQSPIPSU_CONNECTION_MODE_PARALLEL:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is in Dual Parallel connection\r\n");
		} break;

		case XQSPIPSU_CONNECTION_MODE_STACKED:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is in Dual Stack connection\r\n");
		}break;

		default:
		{
			Status = XFSBL_ERROR_INVALID_QSPI_CONNECTION;
			XFsbl_Printf(DEBUG_GENERAL,
					"XFSBL_ERROR_INVALID_QSPI_CONNECTION\r\n");
		}
		break;

	}

	if(Status != XFSBL_SUCCESS) {
		UStatus = (u32)Status;
		goto END;
	}


	switch (QspiPsuInstance.Config.BusWidth) {

		case XFSBL_QSPI_BUSWIDTH_ONE:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is using 1 bit bus\r\n");
			ReadCommand = FAST_READ_CMD_32BIT;
		} break;

		case XFSBL_QSPI_BUSWIDTH_TWO:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is using 2 bit bus\r\n");
			ReadCommand = DUAL_READ_CMD_32BIT;
		} break;

		case XFSBL_QSPI_BUSWIDTH_FOUR:
		{
			XFsbl_Printf(DEBUG_INFO,"QSPI is using 4 bit bus\r\n");
			ReadCommand = QUAD_READ_CMD_32BIT;
			EraseCommand = SEC_ERASE_CMD_4B;
			WriteCommand = QUAD_WRITE_CMD_4B;
		}break;

	}

	/**
	 * Read Flash ID and extract Manufacture and Size information
	 */
	UStatus = FlashReadID(&QspiPsuInstance);
	if (UStatus != XFSBL_SUCCESS) {
		goto END;
	}

	if (MacronixFlash == 1U) {
		if (QspiPsuInstance.Config.BusWidth == XFSBL_QSPI_BUSWIDTH_FOUR) {
			ReadCommand = QUAD_READ_CMD_24BIT2;
		}

		if (QspiPsuInstance.Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			XQspiPsu_SelectFlash(&QspiPsuInstance,
						XQSPIPSU_SELECT_FLASH_CS_BOTH,
						XQSPIPSU_SELECT_FLASH_BUS_BOTH);
			Status = MacronixEnable4B(&QspiPsuInstance);
		} else {
			XQspiPsu_SelectFlash(&QspiPsuInstance,
						XQSPIPSU_SELECT_FLASH_CS_LOWER,
						XQSPIPSU_SELECT_FLASH_BUS_LOWER);
			Status = MacronixEnable4B(&QspiPsuInstance);
			if (Status != XFSBL_SUCCESS) {
				UStatus = XFSBL_FAILURE;
				goto END;
			}
			if (QspiPsuInstance.Config.ConnectionMode ==
					XQSPIPSU_CONNECTION_MODE_STACKED) {
				XQspiPsu_SelectFlash(&QspiPsuInstance,
						XQSPIPSU_SELECT_FLASH_CS_UPPER,
						XQSPIPSU_SELECT_FLASH_BUS_LOWER);
				Status = MacronixEnable4B(&QspiPsuInstance);
			}
		}
	}
	/**
	 * add code: For a Stacked connection, read second Flash ID
	 */
	 QspiMode = QspiPsuInstance.Config.ConnectionMode;
	if ((QspiMode ==
			(s32)(XQSPIPSU_CONNECTION_MODE_PARALLEL)) ||
			(QspiMode ==
					(s32)(XQSPIPSU_CONNECTION_MODE_STACKED)) ) {
		QspiFlashSize = 2 * QspiFlashSize;
	}

	MicronEnable4B(&QspiPsuInstance);

	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Set WP pin down to disable access to status register.\r\n");
	XQspiPsu_WriteProtectToggle(&QspiPsuInstance, 0);

END:
	return UStatus;
}

static u32 MicronEnable4B(XQspiPsu *QspiPsuPtr) {
	s32 Status;
	u32 UStatus;

	XFsbl_Printf(DEBUG_PRINT_ALWAYS,"Enable 4 byte mode.\r\n");

	/*Enable register write*/
	TxBfrPtr = WRITE_ENABLE_CMD;
	FlashMsg[0].TxBfrPtr = &TxBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, &FlashMsg[0], 1);
	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_FAILURE;
		goto END;
	}

	/*Enable 4 byte mode*/
	TxBfrPtr = 0xB7;
	FlashMsg[0].TxBfrPtr = &TxBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, &FlashMsg[0], 1);
	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_FAILURE;
		goto END;
	}
	XFsbl_Printf(DEBUG_PRINT_ALWAYS,"ENABLE_4BYTE_DONE\r\n");

	UStatus = XFSBL_SUCCESS;

END:
	return UStatus;
}

/******************************************************************************
*
* Static API used for Macronix flash to enable 4BYTE mode
*
* @param	QspiPsuPtr Pointer to QSPI instance.
*
* @return	XFSBL_SUCCESS if success, otherwise XFSBL_FAILURE.
*
* @note		None.
*
******************************************************************************/
static u32 MacronixEnable4B(XQspiPsu *QspiPsuPtr)
{
	s32 Status;
	u32 UStatus;

	XFsbl_Printf(DEBUG_GENERAL,"MACRONIX_FLASH_MODE\r\n");

	/*Enable register write*/
	TxBfrPtr = WRITE_ENABLE_CMD;
	FlashMsg[0].TxBfrPtr = &TxBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, &FlashMsg[0], 1);
	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_FAILURE;
		goto END;
	}

	/*Enable 4 byte mode*/
	TxBfrPtr = 0xB7;
	FlashMsg[0].TxBfrPtr = &TxBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(QspiPsuPtr, &FlashMsg[0], 1);
	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_FAILURE;
		goto END;
	}
	XFsbl_Printf(DEBUG_GENERAL,"MACRONIX_ENABLE_4BYTE_DONE\r\n");

	UStatus = XFSBL_SUCCESS;

END:
	return UStatus;
}

/******************************************************************************
*
* Static API used for Macronix flash to enable or disable QPI mode
*
* @param	QspiPsuPtr Pointer to QSPI instance.
* @param    Enable valid values are 0 (disable) and 1 (enable).
*
* @return	XFSBL_SUCCESS if success, otherwise XFSBL_ERROR_QSPI_READ.
*
* @note		None.
*
******************************************************************************/
static u32 MacronixEnableQPIMode(XQspiPsu *QspiPsuPtr, int Enable)
{
	s32 Status;
	u32 UStatus;

	/*Enable register write*/
	TxBfrPtr = WRITE_ENABLE_CMD;
	FlashMsg[0].TxBfrPtr = &TxBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	if (Enable == ENABLE_QPI) {
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	} else {
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
	}
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 1);
	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_ERROR_QSPI_READ;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
		goto END;
	}

	if (Enable == ENABLE_QPI)
		TxBfrPtr = 0x35;
	else
		TxBfrPtr = 0xF5;
	FlashMsg[0].TxBfrPtr = &TxBfrPtr;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	if (Enable == ENABLE_QPI) {
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	} else {
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
	}
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 1);
	if (Status != XFSBL_SUCCESS) {
		UStatus = XFSBL_ERROR_QSPI_READ;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
		goto END;
	}

	UStatus = XFSBL_SUCCESS;

END:
	return UStatus;
}

/*****************************************************************************/
/**
 * This function is used to copy the data from QSPI flash to destination
 * address
 *
 * @param SrcAddress is the address of the QSPI flash where copy should
 * start from
 *
 * @param DestAddress is the address of the destination where it
 * should copy to
 *
 * @param Length Length of the bytes to be copied
 *
 * @return
 * 		- XFSBL_SUCCESS for successful copy
 * 		- errors as mentioned in xfsbl_error.h
 *
 *****************************************************************************/
u32 XFsbl_Qspi32Copy(u32 SrcAddress, PTRSIZE DestAddress, u32 Length)
{
	s32 Status;
	u32 QspiAddr;
	u32 RemainingBytes;
	u32 TransferBytes;
	u32 DiscardByteCnt;
	u32 UStatus;

	XFsbl_Printf(DEBUG_INFO,"QSPI Reading Src 0x%0lx, Dest %0lx, Length %0lx\r\n",
			SrcAddress, DestAddress, Length);

	/**
	 * Check the read length with Qspi flash size
	 */
	if ((SrcAddress + Length) > QspiFlashSize)
	{
		UStatus = XFSBL_ERROR_QSPI_LENGTH;
		XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_LENGTH\r\n");
		goto END;
	}


	/**
	 * Update no of bytes to be copied
	 */
	RemainingBytes = Length;

	/* This delay is needed to get Zynq boot up on 4bit mode without debug. */
	usleep(1);

	while(RemainingBytes > 0U) {

		if (RemainingBytes > DMA_DATA_TRAN_SIZE)
		{
			TransferBytes = DMA_DATA_TRAN_SIZE;
		} else {
			TransferBytes = RemainingBytes;
		}

		/**
		 * Translate address based on type of connection
		 * If stacked assert the slave select based on address
		 */
		QspiAddr = XFsbl_GetQspiAddr((u32 )SrcAddress);

		XFsbl_Printf(DEBUG_INFO,".");
		XFsbl_Printf(DEBUG_DETAILED,
					"QSPI Read Src 0x%0lx, Dest %0lx, Length %0lx\r\n",
						QspiAddr, DestAddress, TransferBytes);

		/**
		 * Setup the read command with the specified address and data for the
		 * Flash
		 */
		if ((MacronixFlash == 1U) &&
				(QspiPsuInstance.Config.BusWidth == XFSBL_QSPI_BUSWIDTH_FOUR)) {

			/* Enable QPI mode */
			Status = MacronixEnableQPIMode(&QspiPsuInstance, ENABLE_QPI);
			if (Status != XFSBL_SUCCESS) {
				UStatus = XFSBL_ERROR_QSPI_READ;
				XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
				goto END;
			}

			/*Command*/
			WriteBuffer[COMMAND_OFFSET]   = (u8)ReadCommand;
			DiscardByteCnt = 1;
			/*MACRONIX is in QPI MODE 4-4-4*/
			FlashMsg[0].TxBfrPtr = WriteBuffer;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = DiscardByteCnt;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;
			/*Address*/
			WriteBuffer[ADDRESS_1_OFFSET] = (u8)((QspiAddr & 0xFF000000U) >> 24);
			WriteBuffer[ADDRESS_2_OFFSET] = (u8)((QspiAddr & 0xFF0000U) >> 16);
			WriteBuffer[ADDRESS_3_OFFSET] = (u8)((QspiAddr & 0xFF00U) >> 8);
			WriteBuffer[ADDRESS_4_OFFSET] = (u8)(QspiAddr & 0xFFU);
			DiscardByteCnt = 4;

			FlashMsg[1].TxBfrPtr = &WriteBuffer[ADDRESS_1_OFFSET];
			FlashMsg[1].RxBfrPtr = NULL;
			FlashMsg[1].ByteCount = DiscardByteCnt;
			FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
			FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_TX;

			/*Dummy*/
			/*Default Dummy is 6 when QPI READ MODE(ECH)*/
			FlashMsg[2].TxBfrPtr = NULL;
			FlashMsg[2].RxBfrPtr = NULL;
			FlashMsg[2].ByteCount = DUMMY_CLOCKS_MACRONIX;
			FlashMsg[2].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
			FlashMsg[2].Flags = 0U;

			/*Data*/
			FlashMsg[3].TxBfrPtr = NULL;
			FlashMsg[3].RxBfrPtr = (u8 *)DestAddress;
			FlashMsg[3].ByteCount = TransferBytes;
			FlashMsg[3].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
			FlashMsg[3].Flags = XQSPIPSU_MSG_FLAG_RX;
			if(QspiPsuInstance.Config.ConnectionMode ==
					XQSPIPSU_CONNECTION_MODE_PARALLEL){
				FlashMsg[3].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
			}

			Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 4);
			if (Status != XFSBL_SUCCESS) {
				UStatus = XFSBL_ERROR_QSPI_READ;
				XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
				goto END;
			}

			/* Disable QPI mode */
			Status = MacronixEnableQPIMode(&QspiPsuInstance, DISABLE_QPI);
			if (Status != XFSBL_SUCCESS) {
				UStatus = XFSBL_ERROR_QSPI_READ;
				XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
				goto END;
			}

		} else {
			WriteBuffer[COMMAND_OFFSET]   = (u8)ReadCommand;
			WriteBuffer[ADDRESS_1_OFFSET] = (u8)((QspiAddr & 0xFF000000U) >> 24);
			WriteBuffer[ADDRESS_2_OFFSET] = (u8)((QspiAddr & 0xFF0000U) >> 16);
			WriteBuffer[ADDRESS_3_OFFSET] = (u8)((QspiAddr & 0xFF00U) >> 8);
			WriteBuffer[ADDRESS_4_OFFSET] = (u8)(QspiAddr & 0xFFU);
			DiscardByteCnt = 5;

			FlashMsg[0].TxBfrPtr = WriteBuffer;
			FlashMsg[0].RxBfrPtr = NULL;
			FlashMsg[0].ByteCount = DiscardByteCnt;
			FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

			/*
			 * It is recommended to have a separate entry for dummy
			 */
			if ((ReadCommand == FAST_READ_CMD_32BIT) ||
					(ReadCommand == DUAL_READ_CMD_32BIT) ||
					(ReadCommand == QUAD_READ_CMD_32BIT)) {

				/* Update Dummy cycles as per flash specs for QUAD IO */

				/*
				 * It is recommended that Bus width value during dummy
				 * phase should be same as data phase
				 */
				if (ReadCommand == FAST_READ_CMD_32BIT) {
					FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
				}

				if (ReadCommand == DUAL_READ_CMD_32BIT) {
					FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_DUALSPI;
				}

				if (ReadCommand == QUAD_READ_CMD_32BIT) {
					FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
				}

				FlashMsg[1].TxBfrPtr = NULL;
				FlashMsg[1].RxBfrPtr = NULL;
				FlashMsg[1].ByteCount = DUMMY_CLOCKS;
				FlashMsg[1].Flags = 0U;
			}

			if (ReadCommand == FAST_READ_CMD_32BIT) {
				FlashMsg[2].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
			}

			if (ReadCommand == DUAL_READ_CMD_32BIT) {
				FlashMsg[2].BusWidth = XQSPIPSU_SELECT_MODE_DUALSPI;
			}

			if (ReadCommand == QUAD_READ_CMD_32BIT) {
				FlashMsg[2].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
			}

			FlashMsg[2].TxBfrPtr = NULL;
			FlashMsg[2].RxBfrPtr = (u8 *)DestAddress;
			FlashMsg[2].ByteCount = TransferBytes;
			FlashMsg[2].Flags = XQSPIPSU_MSG_FLAG_RX;

			if(QspiPsuInstance.Config.ConnectionMode ==
					XQSPIPSU_CONNECTION_MODE_PARALLEL){
				FlashMsg[2].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
			}

			/**
			 * Send the read command to the Flash to read the specified number
			 * of bytes from the Flash, send the read command and address and
			 * receive the specified number of bytes of data in the data buffer
			 */
			Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 3);
			if (Status != XFSBL_SUCCESS) {
				UStatus = XFSBL_ERROR_QSPI_READ;
				XFsbl_Printf(DEBUG_GENERAL,"XFSBL_ERROR_QSPI_READ\r\n");
				goto END;
			}
		}
		/**
		 * Update the variables
		 */
		RemainingBytes -= TransferBytes;
		DestAddress += TransferBytes;
		SrcAddress += TransferBytes;

	}
	UStatus = XFSBL_SUCCESS;
END:
	return UStatus;
}

/*****************************************************************************/
/**
 * This function is used to release the Qspi settings
 *
 * @param	None
 *
 * @return	None
 *
 *****************************************************************************/
u32 XFsbl_Qspi32Release(void)
{
	u32 Status = XFSBL_SUCCESS;

	return Status;
}

/*****************************************************************************/
/**
 *
 * This function erases the sectors in the  serial Flash connected to the
 * QSPIPSU interface.
 *
 * @param	Address contains the address of the first sector which needs to
 *		be erased.
 * @param	ByteCount contains the total size to be erased.
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int FlashErase(u32 Address, u32 ByteCount)
{
	u8 WriteEnableCmd;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	int Sector;
	u32 RealAddr;
	u32 NumSect;
	int Status;
	u32 SectSize;
	u32 SectMask;
	u32 DelayCount;

	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Erase flash from address 0x%lX. Lenght is %d.\n\r", Address, ByteCount);

	WriteEnableCmd = WRITE_ENABLE_CMD;

	if(QspiPsuInstance.Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_PARALLEL) {
		SectMask = (Flash_Config_Table[FCTIndex]).SectMask - (Flash_Config_Table[FCTIndex]).SectSize;
		SectSize = (Flash_Config_Table[FCTIndex]).SectSize * 2;
		NumSect = (Flash_Config_Table[FCTIndex]).NumSect;
	} else if (QspiPsuInstance.Config.ConnectionMode == XQSPIPSU_CONNECTION_MODE_STACKED) {
		NumSect = (Flash_Config_Table[FCTIndex]).NumSect * 2;
		SectMask = (Flash_Config_Table[FCTIndex]).SectMask;
		SectSize = (Flash_Config_Table[FCTIndex]).SectSize;
	} else {
		SectSize = (Flash_Config_Table[FCTIndex]).SectSize;
		NumSect = (Flash_Config_Table[FCTIndex]).NumSect;
		SectMask = (Flash_Config_Table[FCTIndex]).SectMask;
	}

	/*
	 * If the erase size is less than the total size of the flash, use
	 * sector erase command
	 */

	/*
	 * Calculate no. of sectors to erase based on byte count
	 */
	if (ByteCount % SectSize)
		NumSect = (ByteCount / SectSize) + 1;
	else
		NumSect = ByteCount / SectSize;

	/*
	 * If ByteCount to k sectors,
	 * but the address range spans from N to N+k+1 sectors, then
	 * increment no. of sectors to be erased
	 */

	if (((Address + ByteCount) & SectMask) ==
		((Address + (NumSect * SectSize)) & SectMask)) {
		NumSect++;
	}

	for (Sector = 0; Sector < NumSect; Sector++) {

		/*
		 * Translate address based on type of connection
		 * If stacked assert the slave select based on address
		 */
		RealAddr = GetRealAddr(&QspiPsuInstance, Address);

		/*
		 * Send the write enable command to the Flash so that it can be
		 * written to, this needs to be sent as a separate
		 * transfer before the write
		 */
		FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		WriteBuffer[COMMAND_OFFSET] = EraseCommand;

		/*
		 * To be used only if 4B address sector erase cmd is
		 * supported by flash
		 */
		if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
			WriteBuffer[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF000000) >> 24);
			WriteBuffer[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteBuffer[ADDRESS_3_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteBuffer[ADDRESS_4_OFFSET] =
					(u8)(RealAddr & 0xFF);
			FlashMsg[0].ByteCount = 5;
		} else {
			WriteBuffer[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteBuffer[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteBuffer[ADDRESS_3_OFFSET] =
					(u8)(RealAddr & 0xFF);
			FlashMsg[0].ByteCount = 4;
		}

		FlashMsg[0].TxBfrPtr = WriteBuffer;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, FlashMsg, 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Wait for the erase command to be completed
		 */
		DelayCount = 0;
		while (1) {
			if (DelayCount == MAX_DELAY_CNT) {
				Status = XST_FAILURE;
				goto ERROR_PATH;
			} else {
				ReadStatusCmd = StatusCmd;
				FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
				FlashMsg[0].RxBfrPtr = NULL;
				FlashMsg[0].ByteCount = 1;
				FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
				FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

				FlashMsg[1].TxBfrPtr = NULL;
				FlashMsg[1].RxBfrPtr = FlashStatus;
				FlashMsg[1].ByteCount = 2;
				FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
				FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
				if (QspiPsuInstance.Config.ConnectionMode ==
						XQSPIPSU_CONNECTION_MODE_PARALLEL) {
					FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
				}

				Status = XQspiPsu_PolledTransfer(&QspiPsuInstance,
						FlashMsg, 2);
				if (Status != XST_SUCCESS) {
					return XST_FAILURE;
				}

				if (QspiPsuInstance.Config.ConnectionMode ==
						XQSPIPSU_CONNECTION_MODE_PARALLEL) {
					if (FSRFlag) {
						FlashStatus[1] &= FlashStatus[0];
					} else {
						FlashStatus[1] |= FlashStatus[0];
					}
				}

				if (FSRFlag) {
					if ((FlashStatus[1] & 0x80) != 0) {
						break;
					}
				} else {
					if ((FlashStatus[1] & 0x01) == 0) {
						break;
					}
				}

				/* Wait for 1 usec */
				usleep(1);
				DelayCount++;
			}
		}
		Address += SectSize;
	}

ERROR_PATH:
	return Status;
}

/*****************************************************************************/
/**
 *
 * This functions translates the address based on the type of interconnection.
 * In case of stacked, this function asserts the corresponding slave select.
 *
 * @param	QspiPsuPtr is a pointer to the QSPIPSU driver component to use.
 * @param	Address which is to be accessed (for erase, write or read)
 *
 * @return	RealAddr is the translated address - for single it is unchanged;
 *		for stacked, the lower flash size is subtracted;
 *		for parallel the address is divided by 2.
 *
 * @note	In addition to get the actual address to work on flash this
 *		function also selects the CS and BUS based on the configuration
 *		detected.
 *
 ******************************************************************************/
u32 GetRealAddr(XQspiPsu *QspiPsuPtr, u32 Address)
{
	u32 RealAddr;

	switch (QspiPsuPtr->Config.ConnectionMode) {
	case XQSPIPSU_CONNECTION_MODE_SINGLE:
		XQspiPsu_SelectFlash(QspiPsuPtr,
				XQSPIPSU_SELECT_FLASH_CS_LOWER,
				XQSPIPSU_SELECT_FLASH_BUS_LOWER);
		RealAddr = Address;
		break;
	case XQSPIPSU_CONNECTION_MODE_STACKED:
		/* Select lower or upper Flash based on sector address */
		if (Address & Flash_Config_Table[FCTIndex].FlashDeviceSize) {

			XQspiPsu_SelectFlash(QspiPsuPtr,
					XQSPIPSU_SELECT_FLASH_CS_UPPER,
					XQSPIPSU_SELECT_FLASH_BUS_LOWER);
			/*
			 * Subtract first flash size when accessing second flash
			 */
			RealAddr = Address &
				(~Flash_Config_Table[FCTIndex].FlashDeviceSize);
		} else {
			/*
			 * Set selection to L_PAGE
			 */
			XQspiPsu_SelectFlash(QspiPsuPtr,
					XQSPIPSU_SELECT_FLASH_CS_LOWER,
					XQSPIPSU_SELECT_FLASH_BUS_LOWER);

			RealAddr = Address;

		}
		break;
	case XQSPIPSU_CONNECTION_MODE_PARALLEL:
		/*
		 * The effective address in each flash is the actual
		 * address / 2
		 */
		XQspiPsu_SelectFlash(QspiPsuPtr,
				XQSPIPSU_SELECT_FLASH_CS_BOTH,
				XQSPIPSU_SELECT_FLASH_BUS_BOTH);
		RealAddr = Address / 2;
		break;
	default:
		/* RealAddr wont be assigned in this case; */
	break;

	}

	return(RealAddr);
}

/*****************************************************************************/
/**
 *
 * This function writes to the  serial Flash connected to the QSPIPSU interface.
 * All the data put into the buffer must be in the same page of the device with
 * page boundaries being on 256 byte boundaries.
 *
 * @param	Address contains the address to write data to in the Flash.
 * @param	ByteCount contains the number of bytes to write.
 * @param	Command is the command used to write data to the flash. QSPIPSU
 *		device supports only Page Program command to write data to the
 *		flash.
 * @param	WriteBfrPtr is pointer to the write buffer (which is to be transmitted)
 *
 * @return	XST_SUCCESS if successful, else XST_FAILURE.
 *
 * @note	None.
 *
 ******************************************************************************/
int FlashWrite(u32 Address, u32 ByteCount, u8 *WriteBfrPtr)
{
	u8 WriteEnableCmd;
	u8 ReadStatusCmd;
	u8 FlashStatus[2];
	u8 WriteCmd[5];
	u32 RealAddr;
	u32 CmdByteCount;
	int Status;
	u32 DelayCount;
	u32 SentBytes;
	u32 WriteAddr;
	u32 ByteCntForTransfer;

	//XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Write %d bytes to address 0x%lX\n\r", ByteCount, Address);

	WriteEnableCmd = WRITE_ENABLE_CMD;

	SentBytes = 0;
	ByteCntForTransfer = FLASH_PAGE_SIZE;
	WriteAddr = Address;

	while (SentBytes < ByteCount) {
		if ((ByteCount - SentBytes) >= FLASH_PAGE_SIZE) {
			ByteCntForTransfer = FLASH_PAGE_SIZE;
		} else {
			ByteCntForTransfer = ByteCount - SentBytes;
		}

		/*
		 * Translate address based on type of connection
		 * If stacked assert the slave select based on address
		 */
		RealAddr = GetRealAddr(&QspiPsuInstance, WriteAddr);

		/*
		 * Send the write enable command to the Flash so that it can be
		 * written to, this needs to be sent as a separate transfer before
		 * the write
		 */
		FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = 1;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 1);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		WriteCmd[COMMAND_OFFSET] = WriteCommand;

		/* To be used only if 4B address program cmd is supported by flash */
		if (Flash_Config_Table[FCTIndex].FlashDeviceSize > SIXTEENMB) {
			WriteCmd[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF000000) >> 24);
			WriteCmd[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteCmd[ADDRESS_3_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteCmd[ADDRESS_4_OFFSET] =
					(u8)(RealAddr & 0xFF);
			CmdByteCount = 5;
		} else {
			WriteCmd[ADDRESS_1_OFFSET] =
					(u8)((RealAddr & 0xFF0000) >> 16);
			WriteCmd[ADDRESS_2_OFFSET] =
					(u8)((RealAddr & 0xFF00) >> 8);
			WriteCmd[ADDRESS_3_OFFSET] =
					(u8)(RealAddr & 0xFF);
			CmdByteCount = 4;
		}

		FlashMsg[0].TxBfrPtr = WriteCmd;
		FlashMsg[0].RxBfrPtr = NULL;
		FlashMsg[0].ByteCount = CmdByteCount;
		FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
		FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

		FlashMsg[1].TxBfrPtr = WriteBfrPtr + SentBytes;
		FlashMsg[1].RxBfrPtr = NULL;
		FlashMsg[1].ByteCount = ByteCntForTransfer;
		FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_QUADSPI;
		FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_TX;
		if (QspiPsuInstance.Config.ConnectionMode ==
				XQSPIPSU_CONNECTION_MODE_PARALLEL) {
			FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
		}
		Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 2);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		SentBytes = SentBytes + ByteCntForTransfer;
		WriteAddr = WriteAddr + FLASH_PAGE_SIZE;

		/*
		 * Wait for the write command to the Flash to be completed, it takes
		 * some time for the data to be written
		 */
		DelayCount = 0;
		while (1) {
			if (DelayCount == MAX_DELAY_CNT) {
				Status = XST_FAILURE;
				goto ERROR_PATH;
			} else {
				ReadStatusCmd = StatusCmd;
				FlashMsg[0].TxBfrPtr = &ReadStatusCmd;
				FlashMsg[0].RxBfrPtr = NULL;
				FlashMsg[0].ByteCount = 1;
				FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
				FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

				FlashMsg[1].TxBfrPtr = NULL;
				FlashMsg[1].RxBfrPtr = FlashStatus;
				FlashMsg[1].ByteCount = 2;
				FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
				FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
				if (QspiPsuInstance.Config.ConnectionMode ==
						XQSPIPSU_CONNECTION_MODE_PARALLEL) {
					FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
				}
				Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 2);
				if (Status != XST_SUCCESS) {
					return XST_FAILURE;
				}

				if (QspiPsuInstance.Config.ConnectionMode ==
						XQSPIPSU_CONNECTION_MODE_PARALLEL) {
					if (FSRFlag) {
						FlashStatus[1] &= FlashStatus[0];
					} else {
						FlashStatus[1] |= FlashStatus[0];
					}
				}

				if (FSRFlag) {
					if ((FlashStatus[1] & 0x80) != 0) {
						break;
					}
				} else {
					if ((FlashStatus[1] & 0x01) == 0) {
						break;
					}
				}
				/* Wait for 1 usec */
				usleep(1);
				DelayCount++;
			}
		}
	}

ERROR_PATH:
	return Status;
}


int ProtectGoldenImage() {
	u8 StatusCmd;
	u8 WriteEnableCmd;
	u8 FlashStatus[2];
	int Status;

	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "XX1 Protect Golden Image flash sectors if has not been already protected.\n\r");

	StatusCmd = READ_STATUS_CMD;
	FlashMsg[0].TxBfrPtr = &StatusCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	FlashMsg[1].TxBfrPtr = NULL;
	FlashMsg[1].RxBfrPtr = FlashStatus;
	FlashMsg[1].ByteCount = 2;
	FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_RX;
	if (QspiPsuInstance.Config.ConnectionMode ==
			XQSPIPSU_CONNECTION_MODE_PARALLEL) {
		FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
	}
	Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 2);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Status 0 0x%lX, 1 0x%lX\n\r", FlashStatus[0], FlashStatus[1]);

	if ((FlashStatus[1] & GOLDEN_PROTECT_MASK) == 0) {
		XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Protect Golden Image flash sectors.\n\r");
	} else {
		XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Golden Image flash sectors are already protected.\n\r");
		return 0;
	}

	/*
	 * Send the write enable command to the Flash so that it can be
	 * written to, this needs to be sent as a separate transfer before
	 * the write
	 */
	WriteEnableCmd = WRITE_ENABLE_CMD;
	FlashMsg[0].TxBfrPtr = &WriteEnableCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 1);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	FlashStatus[0] = FlashStatus[1] | GOLDEN_PROTECT_MASK;

	XFsbl_Printf(DEBUG_PRINT_ALWAYS, "Write 0x%lX to status register.\n\r", FlashStatus[0]);

	StatusCmd = WRITE_STATUS_CMD;
	FlashMsg[0].TxBfrPtr = &StatusCmd;
	FlashMsg[0].RxBfrPtr = NULL;
	FlashMsg[0].ByteCount = 1;
	FlashMsg[0].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[0].Flags = XQSPIPSU_MSG_FLAG_TX;

	FlashMsg[1].TxBfrPtr = FlashStatus;
	FlashMsg[1].RxBfrPtr = NULL;
	FlashMsg[1].ByteCount = 1;
	FlashMsg[1].BusWidth = XQSPIPSU_SELECT_MODE_SPI;
	FlashMsg[1].Flags = XQSPIPSU_MSG_FLAG_TX;
	if (QspiPsuInstance.Config.ConnectionMode ==
			XQSPIPSU_CONNECTION_MODE_PARALLEL) {
		FlashMsg[1].Flags |= XQSPIPSU_MSG_FLAG_STRIPE;
	}
	Status = XQspiPsu_PolledTransfer(&QspiPsuInstance, &FlashMsg[0], 2);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return 0;
}

#endif /* endof XFSBL_QSPI */
