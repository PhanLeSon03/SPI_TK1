/**
  ******************************************************************************
  * @file    IAP/src/ymodem.c 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    07/27/2009
  * @brief   This file provides all the software functions related to the ymodem 
  *          protocol.
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
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/** @addtogroup IAP
  * @{
  */ 
  
/* Includes ------------------------------------------------------------------*/\
#include <string.h>
#include <stdlib.h>
#include "spi_flash_inf.h"
#include "usbInf.h"
#include "ymodem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t file_name[FILE_NAME_LENGTH], file_name_len;
uint8_t file_size[FILE_SIZE_LENGTH], file_size_len, packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD], *file_ptr, *buf_ptr;
int32_t packet_length, session_done, file_done, packets_received, packet_transmit, errors, session_begin, size = 0;	
uint16_t crc_16;
uint32_t EraseCounter = 0x0;
uint32_t NbrOfPage = 0;
uint32_t RamSource;

uint8_t state;
uint8_t extfile_header[EXT_FLASH_PAGE_SIZE];	
uint32_t r_ExtFlashPage = 0, n_ExtFlashPage =0, extFlashRW_Addr = 0;
uint32_t extFlashDestination = 0; 				/* Ext. Flash user program offset */
extern uint8_t tab_1024[1024];

Ymodem_Receive_Process extDownLoadDevice = {
	ymodem_receive_extflash_init,
	ymodem_receive_extflash_process,
	ymodem_receive_extflash_finish
};

Ymodem_Transmit_Process extUpLoadDevice = {
	ymodem_transmit_extflash_init,
	ymodem_transmit_extflash_process,
	ymodem_transmit_extflash_finish
};
/* Son Added */
Ymodem_Transmit_Process exUpLoadDeviceOpposite = {
	ymodem_transmit_extflash_init_Opposite,
	ymodem_transmit_extflash_process_Opposite,
	ymodem_transmit_extflash_finish_Opposite

};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
uint16_t crc_ymodem_update(uint16_t crc, uint8_t data)
{
	int i;

	crc = crc ^ ((uint16_t)data << 8);
	for (i=0; i<8; i++)
	{
		if(crc & 0x8000)
			crc = (crc << 1) ^ CRC16_POLYNOMIAL;
		else
			crc <<= 1;
	}

	return crc;
}

uint16_t ymodem_crc16_generate(uint8_t *buffer, int32_t size)
{		
	uint16_t crc=0;	
	
	// do CRC checksum
	while(size--)
		crc = crc_ymodem_update(crc, *buffer++);
	
	return crc;
}
	 
/**
  * @brief  Receive byte from sender
  * @param  c: Character
  * @param  timeout: Timeout
  * @retval 0: Byte received
  *         -1: Timeout
  */
static  int32_t receive_byte (uint8_t *c, uint32_t timeout)
{
#if 1
	while (timeout > 0)
	{		
		if (SerialKeyPressed(c) == 1)
		{
			return 0;
		}

		timeout--;
	}
	return -1;
#else
	if (SerialKeyPressed(c) == 1)
	{
		return 0;
	}
	else
	{
		return -1;
	}
#endif
}

/**
  * @brief  Send a byte
  * @param  c: Character
  * @retval 0: Byte sent
  */
static uint32_t send_byte (uint8_t c)
{
	SerialPutChar(c);
	return 0;
}

/**
  * @brief  Receive a packet from sender
  * @param  data
  * @param  length
  * @param  timeout
  *     0: end of transmission
  *    -1: abort by sender
  *    >0: packet length
  * @retval 0: normally return
  *        -1: timeout or packet error
  *         1: abort by user
  */
static int32_t receive_packet (uint8_t *data, int32_t *length, uint32_t timeout)
{
	uint16_t i, packet_size;
	uint8_t c, errors = 0;
	*length = 0;

	while(errors < 3)
	{
		_delay(10);
		if (receive_byte(&c, timeout) != 0)
		{
			return -1;
		}

		switch (c)
		{
			case SOH:
				packet_size = PACKET_SIZE;
				errors = 3;
				break;
			case STX:
				packet_size = PACKET_1K_SIZE;
				errors = 3;
				break;
			case EOT:
				return 0;
			case CA:
				if ((receive_byte(&c, timeout) == 0) && (c == CA))
				{
					*length = -1;
					return 0;
				}
				else
				{
					return -1;
				}
			case ABORT1:
			case ABORT2:
				return 1;
			default:
				errors++;
				//return -1;
				break;
		}
	}	

	*data = c;
	for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++)
	{
		if (receive_byte(data + i, timeout) != 0)
		{
			return -1;
		} 
	}
	
	if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
	{
		return -1;
	}
	
	*length = packet_size;
	return 0;
}

/**
  * @brief  	Transmit a packet to receiver
  * @param  	data
  * @param  	length
  * @param  	timeout    
  * @retval 	0: get an ACK, send next packet
  *        		-1: timeout for a ACK, cancel the transmission         		
  */
static int32_t transmit_packet(uint8_t *data, int32_t length, uint32_t timeout)
{	
	uint8_t c, errors = 0;	

	do{
		// send packet
		SerialPutString(data, length);	

		_delay(20);
		
		if (receive_byte(&c, timeout) != 0)
		{
			return -1;
		}		

		if(c == ACK)
		{
			break;
		}
		else if(c == NAK)
		{
			errors++;
			if(errors == 20)
			{
				return -1;
			}				
		}
		else if(c == CA)
		{
			return -1;
		}				
	} while(errors < 20);
	
	return 0;
}

/**
  * @brief  Receive a file using the ymodem protocol
  * @param  buf: Address of the first byte
  * @retval The size of the file
  */
int32_t ymodemReceive (uint8_t *buf, Ymodem_Receive_Process *pdevice, uint32_t Addr)
{			
	for (session_done = 0, errors = 0, session_begin = 0; ;)
	{
		for (packets_received = 0, file_done = 0, buf_ptr = buf; ;)
		{
			switch (receive_packet(packet_data, &packet_length, YMODEM_COM_TIMEOUT))
			{
				case 0:
					errors = 0;
					switch (packet_length)
					{
						/* Abort by sender */
						case - 1:
							send_byte(ACK);
							return 0;
						/* End of transmission */
						case 0:
							send_byte(ACK);
							file_done = 1;
							break;
						/* Normal packet */
						default:
							if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))
							{
								send_byte(NAK);
							}
							else
							{
								if (packets_received == 0)
								{	/* Filename packet */
									if (packet_data[PACKET_HEADER] != 0)
									{
										/* Filename packet has valid data */
										for (file_name_len = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (file_name_len < FILE_NAME_LENGTH);)
										{
											file_name[file_name_len++] = *file_ptr++;
										}
										file_name[file_name_len++] = '\0';
										
										for (file_size_len = 0, file_ptr ++; (*file_ptr != ' ') && (file_size_len < FILE_SIZE_LENGTH);)
										{
											file_size[file_size_len++] = *file_ptr++;
										}
										file_size[file_size_len++] = '\0';
										
										Str2Int(file_size, &size);

										if(pdevice->pfncProcess_Init(&size, Addr) != 0)
										{
											return -1;
										}
																									
										send_byte(ACK);
										send_byte(CRC16);
									}
									/* Filename packet is empty, end session */
									else
									{
										send_byte(ACK);
										file_done = 1;
										session_done = 1;
										break;
									}
								}
								/* Data packet */
								else
								{
									memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
									RamSource = (uint32_t)buf;	//Son: 	have File Name and File Size information	

									pdevice->pfncProcessing(packet_length, size); //Son : argument doing nothing
									
									send_byte(ACK);
								}
								packets_received ++;
								session_begin = 1;
							}
					}
					break;
				case 1:
					send_byte(CA);
					send_byte(CA);
					return -3;
				default:
					if (session_begin > 0)
					{
						errors ++;
					}
					if (errors > MAX_ERRORS)
					{
						send_byte(CA);
						send_byte(CA);
						return 0;
					}
					send_byte(CRC16);
					break;
			}
			
			if (file_done != 0)
			{
				break;
			}
		}
		
		if (session_done != 0)
		{
			break;
		}
	}

	pdevice->pfncProcess_Finish();
	
	return (int32_t)size;
}

/**
  * @brief  Transmit a file using the ymodem protocol
  * @param  buf: Address of the first byte
  * @retval The size of the file
  */
int32_t ymodemTransmit(Ymodem_Transmit_Process *pdevice, uint32_t Addr)
{			
	uint8_t ch;
	uint16_t i, j;
	
	state = YMODEM_START_STAGE;
	while(1)
	{
		switch(state)
		{
			case YMODEM_START_STAGE:			
				if (receive_byte(&ch, YMODEM_COM_TIMEOUT*0x100) != 0)
				{
					state = YMODEM_TIMEOUT_STAGE;
				}
				else
				{
					if(ch == 'C')
					{						
						packet_transmit = 0;
						state = YMODEM_TRANSMIT_STAGE;
					}
					else
					{
						errors ++;
						if(errors > MAX_ERRORS)
						{
							state = YMODEM_TIMEOUT_STAGE;
						}
					}
				}
				break;

			case YMODEM_TRANSMIT_STAGE:
				if(!packet_transmit)			// send block 0
				{	
					j = 0;

					// copy header
					packet_data[j++] = SOH;
					packet_data[j++] = packet_transmit++;
					packet_data[j++] = ~(packet_data[1]);

					pdevice->pfncProcess_Init(file_name, file_size, Addr);

					Str2Int(file_size, &size);

					// copy file name
					for(i = 0; i < file_name_len; i++)
					{
						packet_data[j++] = file_name[i];
					}

					// copy file size
					for(i = 0; i < file_size_len; i++)
					{
						packet_data[j++] = file_size[i];
					}

					// padding zero
					for(i = j ; i < (PACKET_SIZE + PACKET_HEADER); i++)
					{
						packet_data[j++] = 0x0;
					}

					// calculate crc 16
					crc_16 = ymodem_crc16_generate((uint8_t*)(packet_data + PACKET_HEADER), PACKET_SIZE);

					// copy 2 byte crc 16
					packet_data[j++] = (uint8_t)(crc_16 >> 8);
					packet_data[j++] = (uint8_t)(crc_16 & 0xFF);				
										
					if(transmit_packet(packet_data, j, YMODEM_COM_TIMEOUT) != 0)
					{
						state = YMODEM_TIMEOUT_STAGE;
					}
				}
				else
				{		
					int8_t EOT_flag = 0;
					
					j = 0;
					// copy header
					packet_data[j++] = STX;
					packet_data[j++] = packet_transmit++;
					packet_data[j++] = ~(packet_data[1]);					

					EOT_flag = pdevice->pfncProcessing((uint8_t*)(packet_data + PACKET_HEADER), &packet_length);

					j += packet_length;

					// padding zero if packet_length < PACKET_1K_SIZE
					for(i = j ; i < (PACKET_1K_SIZE + PACKET_HEADER); i++)
					{
						packet_data[j++] = 0x00;
					}

					// calculate crc 16
					crc_16 = ymodem_crc16_generate((uint8_t*)(packet_data + PACKET_HEADER), PACKET_1K_SIZE);

					// copy 2 byte crc 16
					packet_data[j++] = (uint8_t)(crc_16 >> 8);
					packet_data[j++] = (uint8_t)(crc_16 & 0xFF);				
										
					if(transmit_packet(packet_data, j, YMODEM_COM_TIMEOUT) != 0)
					{
						state = YMODEM_TIMEOUT_STAGE;
					}

					if(EOT_flag)	// end of transmission
					{									
						// copy header
						packet_data[0] = EOT;						
						
						if(transmit_packet(&packet_data[0], 1, YMODEM_COM_TIMEOUT) != 0)
						{
							state = YMODEM_TIMEOUT_STAGE;
						}
						else
						{	
							j = 0;
							// copy header
							packet_data[j++] = SOH;
							packet_data[j++] = 0x0;
							packet_data[j++] = ~(packet_data[1]);

							// padding zero
							for(i = j ; i < (PACKET_SIZE + PACKET_HEADER); i++)
							{
								packet_data[j++] = 0x0;
							}

							// calculate crc 16
							crc_16 = ymodem_crc16_generate((uint8_t*)(packet_data + PACKET_HEADER), PACKET_SIZE);

							// copy 2 byte crc 16
							packet_data[j++] = (uint8_t)(crc_16 >> 8);
							packet_data[j++] = (uint8_t)(crc_16 & 0xFF);	

							if(transmit_packet(packet_data, j, YMODEM_COM_TIMEOUT) != 0)
							{
								state = YMODEM_TIMEOUT_STAGE;
							}
							else
							{
								_delay(10);
								while(receive_byte(&ch, YMODEM_COM_TIMEOUT) == 0);	// flush redundant data 
								return (int32_t)size;
							}												
						}
					}
				}
				break;

			case YMODEM_TIMEOUT_STAGE:
			case YMODEM_CANCEL_STAGE:
			case YMODEM_ABORT_STAGE:	
				send_byte(CA);
				send_byte(CA);
				return -1;

			default:
				break;
		}
	}	
	
	return 0;  
}

int8_t ymodem_receive_extflash_init(int32_t *flashSize, uint32_t Addr) //Son: int32_t Addr
{
	uint32_t i, j, r, n;		

	/* Test the size of the image to be sent */
	/* Image size is greater than Flash size */
	if ((*flashSize) > (EXT_FLASH_SIZE - 1))
	{
		/* End session */
		send_byte(CA);
		send_byte(CA);
		return -1;
	}
	
	// copy file name into buffer
	for(i = 0; (file_name[i] != '\0'); i++)
	{
		extfile_header[i] = file_name[i];		
	}	
	extfile_header[i] = 0;		//add one Zero
		

	// copy file size into buffer
	for(i = FILE_NAME_LENGTH, j = 0; (file_size[j] != '\0'); )
	{
		extfile_header[i++] = file_size[j++];		
	}
	extfile_header[i] = 0;			//add one Zero		

	// calculate for writing packet into ext.flash	
	r_ExtFlashPage = (*flashSize) % EXT_FLASH_PAGE_SIZE;
	n_ExtFlashPage = (*flashSize) / EXT_FLASH_PAGE_SIZE;	

	r = (*flashSize) % EXT_FLASH_SECTOR_SIZE;
	j = (*flashSize) / EXT_FLASH_SECTOR_SIZE;
	n = (r) ? (j + 1) : (j);

	if(SPI_FlashInf_IsReady())
	{
		// erase some sector to upgrade data
		for(i = 0, j = Addr; i < n; i++, j += EXT_FLASH_SECTOR_SIZE) //Son: EXT_FLASH_FILE_ADDR --> Addr
		{
			SPI_FlashInf_Erase(j);//Son: EXT_FLASH_FILE_ADDR --> j
		}	
		
		// write file name and file size into ext flash
		// TODO: SON: HERE IS THE POSITION NEED CHANGED:"EXT_FLASH_FILE_ADDR"
		extFlashRW_Addr = Addr;  	//Son :EXT_FLASH_FILE_ADDR --> Addr
		SPI_FlashInf_Write(extfile_header, extFlashRW_Addr, FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2);
		extFlashRW_Addr += (FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2);				// remember the position for next write into flash
	}
	else
	{
		/* End session */
		send_byte(CA);
		send_byte(CA);
		return -1;
	}	
	
	return 0;
}

int8_t ymodem_receive_extflash_process(int32_t packetlen, int32_t size)
{	
	if(SPI_FlashInf_IsReady())
	{		
		if(n_ExtFlashPage >= 4)
		{
			SPI_FlashInf_Write( (uint8_t*)RamSource, extFlashRW_Addr, (EXT_FLASH_PAGE_SIZE*4) );
			extFlashRW_Addr += (EXT_FLASH_PAGE_SIZE*4);
			n_ExtFlashPage -= 4;
		}
		else
		{
			SPI_FlashInf_Write( (uint8_t*)RamSource, extFlashRW_Addr, (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage);	
			extFlashRW_Addr += (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage;	// only for testing
		}
	}
	else
	{
		/* End session */
		send_byte(CA);
		send_byte(CA);
		return -1;
	}	
			
	return 0;
}

void ymodem_receive_extflash_finish(void)
{
}

int8_t ymodem_transmit_extflash_init(uint8_t *pfilename, uint8_t *pfileSize, uint32_t Addr)
{	
	uint8_t i;
	int32_t fileSize;

	extFlashRW_Addr = Addr;// Son : EXT_FLASH_FILE_ADDR --> Addr
	if(SPI_FlashInf_IsReady())
	{
		SPI_FlashInf_Read(extfile_header, extFlashRW_Addr, FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2);
		extFlashRW_Addr += (FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2);				// remember the position for next write into flash
	}
	else
	{
		/* End session */
		send_byte(CA);
		send_byte(CA);
		return -1;
	}		

	// copy file name into buffer
	for(file_name_len = 0; (extfile_header[file_name_len] != '\0'); file_name_len++)
	{
		*(pfilename + file_name_len) = extfile_header[file_name_len];		
	}	
	*(pfilename + file_name_len) = 0;		//add one Zero
	file_name_len++;

	// copy file name into buffer
	for(file_size_len = 0, i = FILE_NAME_LENGTH; (extfile_header[i] != '\0'); file_size_len++, i++)
	{
		*(pfileSize + file_size_len) = extfile_header[i];		
	}	
	*(pfileSize + file_size_len) = 0;		//add one Zero
	file_size_len++;

	Str2Int(pfileSize, &fileSize);
	r_ExtFlashPage = fileSize % EXT_FLASH_PAGE_SIZE;
	n_ExtFlashPage = fileSize / EXT_FLASH_PAGE_SIZE;
	
	return 0;
}

int8_t ymodem_transmit_extflash_process(uint8_t *data, int32_t *length)
{
	if(SPI_FlashInf_IsReady())
	{		
		if(n_ExtFlashPage >= 4)
		{
			SPI_FlashInf_Read(data, extFlashRW_Addr, (EXT_FLASH_PAGE_SIZE*4));
			extFlashRW_Addr += (EXT_FLASH_PAGE_SIZE*4);					// remember the position for next write into flash
			n_ExtFlashPage -= 4;
			// TODO: SON : MAY BE WRONG????----> MUST BE : *length = EXT_FLASH_PAGE_SIZE* n_ExtFlashPage
			*length = EXT_FLASH_PAGE_SIZE*4; // Son: 4 --> n_ExtFlashPage
		}
		else
		{
			SPI_FlashInf_Read(data, extFlashRW_Addr, (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage);			
			extFlashRW_Addr += (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage;	// only for testing
			*length = (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage;

			return 1;
		}
	}
	else
	{
		/* End session */
		send_byte(CA);
		send_byte(CA);
		return -1;
	}		
	
	return 0;
}

void ymodem_transmit_extflash_finish(void)
{
}

/*===================================================== */
/* Son: Read Header opposite from last address down to first address      */
/*                                                                                                */
/*                                                                                                */
/*                                                                                                */
/*                                                                                                */
/*===================================================== */
int8_t ymodem_transmit_extflash_init_Opposite(uint8_t *pfilename, uint8_t *pfileSize, uint32_t Addr)
{	
	uint16_t i;
	int32_t fileSize;
	uint8_t extfile_header_op[EXT_FLASH_PAGE_SIZE];
	extFlashRW_Addr = Addr;// Son : EXT_FLASH_FILE_ADDR --> Addr
	if(SPI_FlashInf_IsReady())
	{
		SPI_FlashInf_Read(extfile_header_op, extFlashRW_Addr - FILE_NAME_LENGTH - FILE_SIZE_LENGTH -2 + 1, FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2);
		extFlashRW_Addr -= (FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2);				// remember the position for next write into flash
		for (i=0; i<FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2;i++)
		{
			extfile_header[i] = extfile_header_op[FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 -i -1];
		}
	}
	else
	{
		/* End session */
		send_byte(CA);
		send_byte(CA);
		return -1;
	}		

	// copy file name into buffer
	for(file_name_len = 0; (extfile_header[file_name_len] != '\0'); file_name_len++)
	{
		*(pfilename + file_name_len) = extfile_header[file_name_len];		
	}	
	*(pfilename + file_name_len) = 0;		//add one Zero
	file_name_len++;

	// copy file name into buffer
	for(file_size_len = 0, i = FILE_NAME_LENGTH; (extfile_header[i] != '\0'); file_size_len++, i++)
	{
		*(pfileSize + file_size_len) = extfile_header[i];		
	}	
	*(pfileSize + file_size_len) = 0;		//add one Zero
	file_size_len++;

	Str2Int(pfileSize, &fileSize);
	r_ExtFlashPage = fileSize % EXT_FLASH_PAGE_SIZE;
	n_ExtFlashPage = fileSize / EXT_FLASH_PAGE_SIZE;
	
	return 0;
}

/*----------------------------------------------------------*/
/* Son:                                                                                      */
/* Read Data opposite from last address down to first address             */
/*                                                                                             */
/*                                                                                             */
/*                                                                                             */
/*----------------------------------------------------------*/
int8_t ymodem_transmit_extflash_process_Opposite (uint8_t *data, int32_t *length)
{
      uint8_t data1[EXT_FLASH_PAGE_SIZE*4];
	uint16_t i;
	if(SPI_FlashInf_IsReady())
	{		
		if(n_ExtFlashPage >= 4)
		{
			SPI_FlashInf_Read(data1, extFlashRW_Addr -(EXT_FLASH_PAGE_SIZE*4) +1, (EXT_FLASH_PAGE_SIZE*4));
			extFlashRW_Addr -= (EXT_FLASH_PAGE_SIZE*4);					// remember the position for next write into flash
			n_ExtFlashPage -= 4;
			*length = EXT_FLASH_PAGE_SIZE*4;
			for (i=0; i < EXT_FLASH_PAGE_SIZE*4; i++)
			{
				data[i] = data1[EXT_FLASH_PAGE_SIZE*4-1 -i];
			}
		}
		else
		{
			SPI_FlashInf_Read(data1, extFlashRW_Addr -((EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage) + 1, (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage);			
			extFlashRW_Addr -= (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage + r_ExtFlashPage);	// only for testing
			*length = (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage;
			for (i=0; i < ((EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage); i++)
			{
				data[i] = data1[(EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage -1 -i];
			}

			return 1;
		}
	}
	else
	{
		/* End session */
		send_byte(CA);
		send_byte(CA);
		return -1;
	}		
	
	return 0;
}

/*--------------------------------------------*/
/* Son:                                                               */
/*                                                                       */
/*                                                                       */
/*                                                                       */
/*                                                                       */
/*--------------------------------------------*/
void ymodem_transmit_extflash_finish_Opposite(void)
{
}



/**
  * @}
  */

