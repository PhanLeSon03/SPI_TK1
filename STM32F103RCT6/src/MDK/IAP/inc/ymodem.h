/**
  ******************************************************************************
  * @file    IAP/inc/ymodem.h 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    07/27/2009
  * @brief   This file provides all the software function headers of the ymodem.c 
  *          file.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT YYYY STMicroelectronics</center></h2>
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _YMODEM_H_
#define _YMODEM_H_

/* Includes ------------------------------------------------------------------*/
#include "common.h"

/* Exported types ------------------------------------------------------------*/
typedef struct {
	int8_t (*pfncProcess_Init)(int32_t*,uint32_t);        		/* Initialize the process */	
	int8_t (*pfncProcessing)(int32_t,int32_t);		/* Device dependent process after the initital stage */
	void (*pfncProcess_Finish)(void);				/* finishing the process */
} Ymodem_Receive_Process;

typedef struct {
	int8_t (*pfncProcess_Init)(uint8_t*, uint8_t*,uint32_t);        	/* Initialize the process */	
	int8_t (*pfncProcessing)(uint8_t*, int32_t*);	/* Device dependent process after the initital stage */
	void (*pfncProcess_Finish)(void);					/* finishing the process */
} Ymodem_Transmit_Process;

/* Exported constants --------------------------------------------------------*/
#define PACKET_SEQNO_INDEX      			(1)
#define PACKET_SEQNO_COMP_INDEX 		(2)

#define PACKET_HEADER           			(3)
#define PACKET_TRAILER          			(2)
#define PACKET_OVERHEAD         			(PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             				(128)
#define PACKET_1K_SIZE          			(1024)

#define FILE_NAME_LENGTH        			(32)
#define FILE_SIZE_LENGTH        			(16)

#define SOH 			(0x01)  /* start of 128-byte data packet */
#define STX                 (0x02)  /* start of 1024-byte data packet */
#define EOT            	(0x04)  /* end of transmission */
#define ACK               	(0x06)  /* acknowledge */
#define NAK                	(0x15)  /* negative acknowledge */
#define CA                 	(0x18)  /* two of these in succession aborts transfer */
#define CRC16           	(0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1       	(0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2        	(0x61)  /* 'a' == 0x61, abort by user */

#define YMODEM_COM_TIMEOUT 	(0x100000)
#define MAX_ERRORS      	(20)

#define CRC16_POLYNOMIAL (0x1021)


#define YMODEM_START_STAGE 			0
#define YMODEM_TRANSMIT_STAGE 		1
#define YMODEM_TIMEOUT_STAGE 			2
#define YMODEM_CANCEL_STAGE 			3
#define YMODEM_ABORT_STAGE 			4
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int32_t ymodemReceive(uint8_t *buf, Ymodem_Receive_Process *pdevice, uint32_t Addr);
int32_t ymodemTransmit(Ymodem_Transmit_Process *pdevice, uint32_t Addr);

uint16_t crc_ymodem_update(uint16_t crc, uint8_t data);
uint16_t ymodem_crc16_generate(uint8_t *buffer, int32_t size);
int8_t ymodem_receive_interflash_init(int32_t *flashSize);
int8_t ymodem_receive_interflash_process(int32_t packetlen, int32_t size);
void ymodem_receive_interflash_finish(void);
int8_t ymodem_receive_extflash_init(int32_t *flashSize, uint32_t Addr);
int8_t ymodem_receive_extflash_process(int32_t packetlen, int32_t size);
void ymodem_receive_extflash_finish(void);
int8_t ymodem_transmit_extflash_init(uint8_t *pfilename, uint8_t *pfileSize,uint32_t Addr);
int8_t ymodem_transmit_extflash_process(uint8_t *data, int32_t *length);
void ymodem_transmit_extflash_finish(void);
/* Son Add */
int8_t ymodem_transmit_extflash_init_Opposite(uint8_t *pfilename, uint8_t *pfileSize,uint32_t Addr);
int8_t ymodem_transmit_extflash_process_Opposite(uint8_t *data, int32_t *length);
void ymodem_transmit_extflash_finish_Opposite(void);
#endif  /* _YMODEM_H_ */

