/**************************************************************************************************************
* Copyright 2009 TEKBOX DIGITAL SOLUTIONS Company
* All Rights Reserved
*
* Module name	: bsp_spi.h
* Author			: Phung The Vu.
* Version            	: V1.0.0
* Date               	: 17/05/2010
*
* Module Description:
* This file contains all the functions prototypes for the  spi peripheral.
*
**************************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_SPI_H
#define __BSP_SPI_H

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Extern declaration ------------------------------------------------------------*/
void bspSPI_CS_Low(void);
void bspSPI_CS_High(void);
void bspSPI_Init(void);
void bspSPI1_Init(void);


uint8_t bspSPI_ReadByte(void);
//uint8_t bspSPI_SendByte_Son(uint8_t byte);
uint8_t bspSPI_SendByte(uint8_t byte);

bool SPI2_Receive(uint8_t * RxBuf, uint16_t Len , uint16_t TimeOut);
bool SPI1_Receive(uint8_t * RxBuf, uint16_t Len , uint16_t TimeOut);


#endif /* __BSP_SPI_H */

