/**************************************************************************************************************
* Copyright 2009 TEKBOX DIGITAL SOLUTIONS Company
* All Rights Reserved
*
* Module name	: spi_flash_inf.h
* Author			: Phung The Vu.
* Version            	: V1.0.0
* Date               	: 17/05/2010
*
* Module Description:
* This file contains all the functions prototypes for the  spi flash driver.
*
**************************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_FLASH_INF_H
#define __SPI_FLASH_INF_H

#define __TEST_FLASH_DRIVER_

/* Includes ------------------------------------------------------------------*/
#include "bsp_spi.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#if !defined (S25FL064P) && !defined (M25P64)
  	//#define M25P64   			/*!< ST 8Mbyte flash */
  	#define S25FL064P   		/*!< Spansion 8Mbyte flash */
#else 
 #error "Please select first the serial flash device to be used in application"    
#endif

#define EXT_FLASH_PAGE_SIZE                  	(0x100) 		/* 256Bytes page size */
#define EXT_FLASH_SECTOR_SIZE			0x10000 
#define EXT_FLASH_FILE_ADDR			0x10000 
#define EXT_FLASH_FILE_ADDR2                 (0x210000)
#define EXT_FLASH_FILE_ADDR3                 (0x410000)
#define EXT_FLASH_FILE_ADDR4                 (0x610000)
#define EXT_FLASH_SIZE                        	(0x800000) 	/* 8MBytes */
 
/*----- High layer function -----*/
/* Initialize SPI FLASH */
#define SPI_FLASH_Init()       				bspSPI_Init()

/*----- Low layer function -----*/
/* Select SPI FLASH: Chip Select pin low  */
#define SPI_FLASH_CS_LOW()       			bspSPI_CS_Low()
/* Deselect SPI FLASH: Chip Select pin high */
#define SPI_FLASH_CS_HIGH()   			bspSPI_CS_High()
/* Deselect SPI FLASH: Chip Select pin high */
#define SPI_FLASH_ReadByte()   			bspSPI_ReadByte()
/* Deselect SPI FLASH: Chip Select pin high */
#define SPI_FLASH_SendByte(byte)   		bspSPI_SendByte((uint8_t)(byte))

/* Exported functions ------------------------------------------------------- */
/*----- C interface -----*/
uint16_t SPI_FlashInf_Init(void);
uint8_t SPI_FlashInf_IsReady(void);
#ifdef S25FL064P
uint16_t SPI_FlashInf_Erase4K(uint32_t SectorAddress);
uint16_t SPI_FlashInf_Erase8K(uint32_t SectorAddress);
#endif

uint16_t SPI_FlashInf_Erase (uint32_t SectorAddress);
uint16_t SPI_FlashInf_Erase_All(void);
uint16_t SPI_FlashInf_Write(uint8_t* pBuffer, uint32_t WriteAddress, uint32_t DataLength);
uint8_t SPI_FlashInf_Read (uint8_t* pBuffer, uint32_t ReadAddress, uint32_t DataLength);
uint8_t SPI_Check_Manufacturer_ID(void);

#ifdef __TEST_FLASH_DRIVER_
void Test_Flash_Page_to_Page(uint8_t mode);
#endif

/*----- High layer function -----*/
#ifdef S25FL064P
void SPI_FLASH_SubSectorErase4K(uint32_t SectorAddr);
void SPI_FLASH_SubSectorErase8K(uint32_t SectorAddr);
#endif

void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);

/*----- Low layer function -----*/
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif /* __SPI_FLASH_INF_H */

