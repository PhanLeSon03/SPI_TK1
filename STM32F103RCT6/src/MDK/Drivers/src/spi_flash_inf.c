/**************************************************************************************************************
* Copyright 2009 TEKBOX DIGITAL SOLUTIONS Company
* All Rights Reserved
*
* Module name	: spi_flash_inf.c
* Author			: Phung The Vu.
* Version            	: V1.0.0
* Date               	: 17/05/2010
*
* Module Description:
* This file provides a set of functions needed to manage the
*                      communication between SPI peripheral and SPI M25P64 FLASH.
*
**************************************************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "spi_flash_inf.h"

/* Private typedef -----------------------------------------------------------*/
#define SPI_FLASH_PageSize    0x100

/* Private define ------------------------------------------------------------*/
#define WRSR       	0x01  /* Write Status Register instruction */
#define WREN       	0x06  /* Write enable instruction */
#define PP      		0x02  /* Write to Memory instruction */

#define READ       		0x03  /* Read from Memory instruction */
#define RDSR       		0x05  /* Read Status Register instruction  */
#define RDID       		0x9F  /* Read identification */

#ifdef S25FL064P
 #define P4E        		0x20  /* 4 KB Parameter Sector Erase */
 #define P8E        		0x40  /* 8 KB (two 4 KB) Parameter Sector Erase */
#endif

#define SE         		0xD8  /* Sector Erase instruction */
#define BE         		0xC7  /* Bulk Erase instruction */

#define WIP_Flag   	0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte 0xA5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : SPI_FlashInf_Init
* Description    : Initializes the Media on the STM32
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t SPI_FlashInf_Init(void)
{
	SPI_FLASH_Init();
	return 0;
}

uint8_t SPI_FlashInf_IsReady(void)
{
	int8_t ret = 0;
	uint32_t timeout = 0x1FFF;

	while( (SPI_Check_Manufacturer_ID() == 0) && (timeout))
	{
		timeout--;
	}

	if(timeout)
	{
		ret = 1;
	}

	return ret;
}

#ifdef S25FL064P
/*******************************************************************************
* Function Name  : SPI_FlashInf_Erase4K
* Description    : Erase sector and the SectorAddr must be multiple of 2^12.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t SPI_FlashInf_Erase4K(uint32_t SectorAddress)
{
	SPI_FLASH_SubSectorErase4K(SectorAddress);
	return 0;
}

/*******************************************************************************
* Function Name  : SPI_FlashInf_Erase8K
* Description    : Erase sector and the SectorAddr must be multiple of 2^12.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t SPI_FlashInf_Erase8K(uint32_t SectorAddress)
{
	SPI_FLASH_SubSectorErase8K(SectorAddress);
	return 0;
}
#endif

/*******************************************************************************
* Function Name  : SPI_FlashInf_Erase
* Description    : Erase sector
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t SPI_FlashInf_Erase(uint32_t SectorAddress)
{
	SPI_FLASH_SectorErase(SectorAddress);
	return 0;
}

/*******************************************************************************
* Function Name  : SPI_FlashInf_Erase
* Description    : Erase sector
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t SPI_FlashInf_Erase_All(void)
{
	SPI_FLASH_BulkErase();
	return 0;
}

/*******************************************************************************
* Function Name  : SPI_FlashInf_Write
* Description    : Write sectors
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t SPI_FlashInf_Write(uint8_t* pBuffer, uint32_t WriteAddress, uint32_t DataLength)
{
	SPI_FLASH_BufferWrite(pBuffer, WriteAddress, (uint16_t)DataLength);
	return 0;
}

/*******************************************************************************
* Function Name  : SPI_FlashInf_Read
* Description    : Read sectors
* Input          : None
* Output         : None
* Return         : buffer address pointer
*******************************************************************************/
uint8_t SPI_FlashInf_Read(uint8_t* pBuffer, uint32_t ReadAddress, uint32_t DataLength)
{
	SPI_FLASH_BufferRead(pBuffer, ReadAddress, (uint16_t)DataLength);
	return 0;
}

uint8_t SPI_Check_Manufacturer_ID(void)
{	
	uint32_t	id = 0;
		
	id = SPI_FLASH_ReadID();

#ifdef S25FL064P
	if( (uint8_t)(id>>16) == 0x01)	// check the manufacture
	{				
		return 1;       	// the manufacture is Spansion
	}
#elif defined(M25P64)
	if( (uint8_t)(id>>16) == 0x20)	// check the manufacture
	{				
		return 1;       	// the manufacture is ST
	}
#endif
			
	return 0;
}

#ifdef __TEST_FLASH_DRIVER_
#include <string.h>
#include "common.h"

uint8_t TestReadBuffer[17] ={0};

void Test_Flash_Page_to_Page(uint8_t mode)
{
	// 0 : test read, write, erase with one sector 64KB
	// 1 : test read, write, erase with one sub-sector 8KB 
	// 2 : test read, write, erase with one sub-sector 4KB
	// Note : sub-sector 8kb and 4kb mode it's just supported by Spansion Flash chip, ST chip is not
	
	uint8_t OneSectorWriteBuffer[]={'t','e','s','t','s','p','i','f','l','a','s','h','1','s','e','c','\0'};	
	uint8_t SubSector4kWriteBuffer[]={'t','e','s','t','s','p','i','f','l','a','s','h','4','k','b','.','\0'};
	uint8_t SubSector8kWriteBuffer[]={'t','e','s','t','s','p','i','f','l','a','s','h','8','k','b','.','\0'};
	uint32_t idx, addr = 0x10000;
	TestReadBuffer[0] = '\0';	
		
	switch(mode)
	{
		case 0:
			SerialPutString("\r\n\r\n Test read-write-erase with one sector 64KB ", 0);
			SerialPutString("\r\nwrite pattern: ", 0);
			SerialPutString(OneSectorWriteBuffer, 0);			
			// erase the sector
			SPI_FlashInf_Erase(addr);
			// write data to the sector
			for(idx = 0; idx < 0x10000;)
			{
				SPI_FlashInf_Write(OneSectorWriteBuffer, addr + idx, 16);
				idx += 16;
			}
			// read data from the sector to check success or failure
			SPI_FlashInf_Read(TestReadBuffer, 0x1FFF0, 16);		
			TestReadBuffer[16] = '\0';
			SerialPutString("\r\nread: ", 0);
			SerialPutString(TestReadBuffer, 0);
			// compare pattern string and read out string
			if(!memcmp(TestReadBuffer, OneSectorWriteBuffer, 16))
			{
				SerialPutString("\r\nTest read-write-erase with one sector 64KB is OK!!! ", 0);
			}
			break;
		case 1:
			SerialPutString("\r\n\r\n Test read-write-erase with one sub-sector 8KB ", 0);
			SerialPutString("\r\nwrite pattern: ", 0);
			SerialPutString(SubSector8kWriteBuffer, 0);						
			// erase the sub-sector 8kb
			SPI_FlashInf_Erase8K(addr);
			// write data to the sector
			for(idx = 0; idx < 0x2000;)
			{
				SPI_FlashInf_Write(SubSector8kWriteBuffer, addr + idx, 16);
				idx += 16;
			}
			// read data from the sector to check success or failure
			SPI_FlashInf_Read(TestReadBuffer, 0x11FF0, 16);	
			TestReadBuffer[16] = '\0';
			SerialPutString("\r\nread: ", 0);
			SerialPutString(TestReadBuffer, 0);	
			// check data of the sector is affected by SPI_FlashInf_Erase8K() command or not
			SerialPutString("\r\ncheck last 16byte of the sector: ", 0);
			SPI_FlashInf_Read(TestReadBuffer, 0x1FFF0, 16);	
			TestReadBuffer[16] = '\0';			
			SerialPutString(TestReadBuffer, 0);
			// compare pattern string and read out string
			if(!memcmp(TestReadBuffer, OneSectorWriteBuffer, 16))
			{
				SerialPutString("\r\nTest read-write-erase with one sub-sector 8KB is OK!!! ", 0);
			}
			break;
		case 2:
			SerialPutString("\r\n\r\n Test read-write-erase with one sub-sector 4KB ", 0);
			SerialPutString("\r\nwrite pattern: ", 0);
			SerialPutString(SubSector4kWriteBuffer, 0);						
			// erase the sub-sector 4kb
			SPI_FlashInf_Erase4K(addr);
			// write data to the sector
			for(idx = 0; idx < 0x1000;)
			{
				SPI_FlashInf_Write(SubSector4kWriteBuffer, addr + idx, 16);
				idx += 16;
			}
			// read data from the sector to check success or failure
			SPI_FlashInf_Read(TestReadBuffer, 0x10FF0, 16);
			TestReadBuffer[16] = '\0';
			SerialPutString("\r\nread: ", 0);
			SerialPutString(TestReadBuffer, 0);
			// check data of the sector is affected by SPI_FlashInf_Erase4K() command or not
			SerialPutString("\r\ncheck last 16byte of the sub-sector 8kb: ", 0);
			SPI_FlashInf_Read(TestReadBuffer, 0x11FF0, 16);		
			TestReadBuffer[16] = '\0';			
			SerialPutString(TestReadBuffer, 0);
			SerialPutString("\r\ncheck last 16byte of the sector: ", 0);
			SPI_FlashInf_Read(TestReadBuffer, 0x1FFF0, 16);		
			TestReadBuffer[16] = '\0';			
			SerialPutString(TestReadBuffer, 0);
			// compare pattern string and read out string
			if(!memcmp(TestReadBuffer, OneSectorWriteBuffer, 16))
			{
				SerialPutString("\r\nTest read-write-erase with one sub-sector 4KB is OK!!! ", 0);
			}
			break;
	}				
}
#endif

#ifdef S25FL064P
/*******************************************************************************
* Function Name  : SPI_FLASH_SubSectorErase4K
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase and the SectorAddr must be multiple of 2^12.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SubSectorErase4K(uint32_t SectorAddr)
{		
	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();

	/* Sector Erase */
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send Sector Erase instruction */
	SPI_FLASH_SendByte(P4E);
	/* Send SectorAddr high nibble address byte */
	SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
	/* Send SectorAddr medium nibble address byte */
	SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
	/* Send SectorAddr low nibble address byte */
	SPI_FLASH_SendByte(SectorAddr & 0xFF);
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SubSectorErase8K
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase and the SectorAddr must be multiple of 2^12.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SubSectorErase8K(uint32_t SectorAddr)
{		
	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();

	/* Sector Erase */
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send Sector Erase instruction */
	SPI_FLASH_SendByte(P8E);
	/* Send SectorAddr high nibble address byte */
	SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
	/* Send SectorAddr medium nibble address byte */
	SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
	/* Send SectorAddr low nibble address byte */
	SPI_FLASH_SendByte(SectorAddr & 0xFF);
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}
#endif

/*******************************************************************************
* Function Name  : SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{		
	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();

	/* Sector Erase */
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send Sector Erase instruction */
	SPI_FLASH_SendByte(SE);
	/* Send SectorAddr high nibble address byte */
	SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
	/* Send SectorAddr medium nibble address byte */
	SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
	/* Send SectorAddr low nibble address byte */
	SPI_FLASH_SendByte(SectorAddr & 0xFF);
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BulkErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BulkErase(void)
{		
	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();

	/* Bulk Erase */
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send Bulk Erase instruction  */
	SPI_FLASH_SendByte(BE);
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single PP
*                  cycle(Page PP sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{	
	/* Enable the write access to the FLASH */
	SPI_FLASH_WriteEnable();

	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	/* Send "Write to Memory " instruction */
	SPI_FLASH_SendByte(PP);
	/* Send WriteAddr high nibble address byte to write to */
	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
	/* Send WriteAddr medium nibble address byte to write to */
	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
	/* Send WriteAddr low nibble address byte to write to */
	SPI_FLASH_SendByte(WriteAddr & 0xFF);

	/* while there is data to be written on the FLASH */
	while (NumByteToWrite--)
	{
		/* Send the current byte */
		SPI_FLASH_SendByte(*pBuffer);
		/* Point on the next byte to be written */
		pBuffer++;
	}

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of PP cycles are reduced, using Page PP sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % SPI_FLASH_PageSize;
	count = SPI_FLASH_PageSize - Addr;
	NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

	if (Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
	{
		if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
		{
			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
		}
		else /* NumByteToWrite > SPI_FLASH_PageSize */
		{
			while (NumOfPage--)
			{
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
				WriteAddr +=  SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}
			
			if (NumOfSingle != 0)
			{
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
			}
		}
	}
	else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
	{
		if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
		{
			if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
			{
				temp = NumOfSingle - count;

				SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
				WriteAddr +=  count;
				pBuffer += count;

				SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
			}
			else
			{
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
			}
		}
		else /* NumByteToWrite > SPI_FLASH_PageSize */
		{
			NumByteToWrite -= count;
			NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
			NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

			SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
			WriteAddr +=  count;
			pBuffer += count;

			while (NumOfPage--)
			{
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
				WriteAddr +=  SPI_FLASH_PageSize;
				pBuffer += SPI_FLASH_PageSize;
			}

			if (NumOfSingle != 0)
			{
				SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
			}
		}
	}
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferRead
* Description    : Reads a block of data from the FLASH.
* Input          : - pBuffer : pointer to the buffer that receives the data read
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{	
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();

	/* Send "Read from Memory " instruction */
	SPI_FLASH_SendByte(READ);

	/* Send ReadAddr high nibble address byte to read from */
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	/* Send ReadAddr medium nibble address byte to read from */
	SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
	/* Send ReadAddr low nibble address byte to read from */
	SPI_FLASH_SendByte(ReadAddr & 0xFF);

	while (NumByteToRead--) /* while there is data to be read */
	{
		/* Read a byte from the FLASH */
		*pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
		/* Point to the next location where the byte read will be saved */
		pBuffer++;
	}

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
uint32_t SPI_FLASH_ReadID(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
		
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();

	/* Send "RDID " instruction */
	SPI_FLASH_SendByte(RDID);

	/* Read a byte from the FLASH */
	Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

	/* Read a byte from the FLASH */
	Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

	/* Read a byte from the FLASH */
	Temp2 = SPI_FLASH_SendByte(Dummy_Byte);

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	return Temp;
}

/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr)
{	
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();

	/* Send "Read from Memory " instruction */
	SPI_FLASH_SendByte(READ);

	/* Send the 24-bit address of the address to read from -----------------------*/
	/* Send ReadAddr high nibble address byte */
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	/* Send ReadAddr medium nibble address byte */
	SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
	/* Send ReadAddr low nibble address byte */
	SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();

	/* Send "Write Enable" instruction */
	SPI_FLASH_SendByte(WREN);

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WaitForWriteEnd
* Description    : Polls the status of the Write In Progress (WIP) flag in the
*                  FLASH's status  register  and  loop  until write  opertaion
*                  has completed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WaitForWriteEnd(void)
{
	uint8_t FLASH_Status = 0;

	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();

	/* Send "Read Status Register" instruction */
	SPI_FLASH_SendByte(RDSR);

	/* Loop as long as the memory is busy with a write cycle */
	do
	{
		/* Send a dummy byte to generate the clock needed by the FLASH
		and put the value of the status register in FLASH_Status variable */
		FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
	}
	while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
}

