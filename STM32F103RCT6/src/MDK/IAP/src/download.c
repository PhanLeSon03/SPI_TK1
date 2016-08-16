#include "Download.h"
/**
  ******************************************************************************
  * @file    IAP/src/download.c 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    07/27/2009
  * @brief   This file provides the software which allows to download an image 
  *          to internal Flash.
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
/* Includes ------------------------------------------------------------------*/



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint8_t tab_1024[1024] = {
	0
};


/* Private functions ---------------------------------------------------------*/
void CreateLogFile(uint8_t* Buf, uint32_t Addr, uint8_t *FileName, uint16_t NumByteToWrite);
uint8_t CheckFileExist(uint8_t * FileName, uint32_t Addr);
void CopyData2Buf ( uint32_t Addr,uint32_t Size);
void CopyDataFromBuf(uint32_t Addr,uint32_t Size);

uint8_t CheckFileExistOpposite(uint8_t * FileName, uint32_t Addr);
void Opposite(uint8_t *New,uint8_t *Old, uint16_t BUF_LENGTH);
void CopyData2Buf_Opposite(uint32_t Addr,uint32_t Size);
void CopyDataFromBuf_Opposite(uint32_t Addr,uint32_t Size);
void CreateLogFile_Opposite(uint8_t* Buf, uint32_t Addr, uint8_t *FileName, uint16_t NumByteToWrite);

uint8_t CheckFreeSpace_GPS_GSM(uint16_t Size, uint8_t SelectFile);
uint32_t ReadFileSize_GSM(void);
uint32_t ReadFileSize_GPS(void);

void Copy2Buf_Opposite(uint32_t Addr_Insight,uint32_t Addr_Sector,uint16_t NumByteToFix);
void Copy2Buf(uint32_t Addr_Insight,uint32_t Addr_Sector,uint16_t NumByteToFix);


/**
  * @brief  Display the Menu on to HyperTerminal
  * @param  None
  * @retval None
  */
void extFlashLoader(void)
{
	uint8_t key = 0;
		
	SerialPutString("\r\n================== Main Menu ============================\r\n\n", 0);
	SerialPutString("  Download database ------- 1\r\n\n", 0);
	SerialPutString("  Upload database------------------- 2\r\n\n", 0);
	SerialPutString("==========================================================\r\n\n", 0);	
	
	while (1)
	{
		usbInfRxBufFlush();		// flush usb receive buffer
		key = GetKey();

		if (key == 0x31)
		{
			// for test only 
			UserToPMABufferCopy("1---", ENDP1_TXADDR, 4);			
			SetEPTxCount(ENDP1, 4);
			SetEPTxValid(ENDP1);	
			_delay(20);

			usbInfRxBufFlush();		// flush usb receive buffer
			SerialPutString("\r\n================== Selected Position=================\r\n\n", 0);//Son
	             SerialPutString("  File 1: ------- 1\r\n\n", 0);                                                                   //Son
	             SerialPutString("  File 2:-------- 2\r\n\n", 0);										   //	Son
			SerialPutString("  File 3:-------- 3\r\n\n", 0);										   // Son	
			SerialPutString("  File 4:-------- 4\r\n\n", 0);										   // Son
	             SerialPutString("===================================================\r\n\n", 0);//Son	
	             usbInfRxBufFlush();		// fSon :lush usb receive buffer
		      key = GetKey();                 //Son

			 switch (key)
			 	{
			 	case 0x31 :
						// for test only 
			                   UserToPMABufferCopy("1---", ENDP1_TXADDR, 4);			
			                   SetEPTxCount(ENDP1, 4);
			                   SetEPTxValid(ENDP1);	
						_delay(20);
	
			                   usbInfRxBufFlush();		// flush usb receive buffer
						extSerialDownload(EXT_FLASH_FILE_ADDR);
						break;
				case 0x32 :
						// for test only 
		                     	UserToPMABufferCopy("2---", ENDP1_TXADDR, 4);			
						SetEPTxCount(ENDP1, 4);
						SetEPTxValid(ENDP1);	
						_delay(20);
	
			         
			                   usbInfRxBufFlush();		// flush usb receive buffer
						extSerialDownload(EXT_FLASH_FILE_ADDR2);
						break;
				case 0x33 :
					     // for test only 
		                     	UserToPMABufferCopy("3---", ENDP1_TXADDR, 4);			
						SetEPTxCount(ENDP1, 4);
						SetEPTxValid(ENDP1);	
						_delay(20);
	
		                    	usbInfRxBufFlush();		// flush usb receive buffer
						extSerialDownload(EXT_FLASH_FILE_ADDR3);
						break;
				case 0x34 :
						// for test only 
		                     	UserToPMABufferCopy("4---", ENDP1_TXADDR, 4);			
						SetEPTxCount(ENDP1, 4);
						SetEPTxValid(ENDP1);	
						_delay(20);
	
		                    	usbInfRxBufFlush();		// flush usb receive buffer
						extSerialDownload(EXT_FLASH_FILE_ADDR4);
						break;
				default :
						break;
						
			 	}                
			
		}
		else if (key == 0x32)
		{
			// for test only 
			UserToPMABufferCopy("1---", ENDP1_TXADDR, 4);			
			SetEPTxCount(ENDP1, 4);
			SetEPTxValid(ENDP1);	
			_delay(20);

			usbInfRxBufFlush();		// flush usb receive buffer
			SerialPutString("\r\n================== Selected Position=================\r\n\n", 0);//Son
	             SerialPutString("  File 1: ------- 1\r\n\n", 0);                                                                   //Son
	             SerialPutString("  File 2:-------- 2\r\n\n", 0);										   //	Son
			SerialPutString("  File 3:-------- 3\r\n\n", 0);										   // Son	
			SerialPutString("  File 4:-------- 4\r\n\n", 0);										   // Son
	             SerialPutString("===================================================\r\n\n", 0);//Son	
	             usbInfRxBufFlush();		// fSon :lush usb receive buffer
		      key = GetKey();                 //Son

			switch (key)
			 	{
			 	case 0x31 :
						// for test only 
			                   UserToPMABufferCopy("1---", ENDP1_TXADDR, 4);			
			                   SetEPTxCount(ENDP1, 4);
			                   SetEPTxValid(ENDP1);	
						_delay(20);
	
			                   usbInfRxBufFlush();		// flush usb receive buffer
						extSerialUpload(EXT_FLASH_FILE_ADDR);
						break;
				case 0x32 :
						// for test only 
		                     	UserToPMABufferCopy("2---", ENDP1_TXADDR, 4);			
						SetEPTxCount(ENDP1, 4);
						SetEPTxValid(ENDP1);	
						_delay(20);
	
			         
			                   usbInfRxBufFlush();		// flush usb receive buffer
						extSerialUpload(EXT_FLASH_FILE_ADDR2);
						break;
				case 0x33 :
					     // for test only 
		                     	UserToPMABufferCopy("3---", ENDP1_TXADDR, 4);			
						SetEPTxCount(ENDP1, 4);
						SetEPTxValid(ENDP1);	
						_delay(20);
	
		                    	usbInfRxBufFlush();		// flush usb receive buffer
						extSerialUpload(EXT_FLASH_FILE_ADDR3);
						break;
				case 0x34 :
						// for test only 
		                     	UserToPMABufferCopy("4---", ENDP1_TXADDR, 4);			
						SetEPTxCount(ENDP1, 4);
						SetEPTxValid(ENDP1);	
						_delay(20);
	
		                    	usbInfRxBufFlush();		// flush usb receive buffer
						extSerialUpload(EXT_FLASH_FILE_ADDR4);
						break;
				default :
						break;
						
			 	}         
					
		}		
		else
		{
			SerialPutString("Invalid Number ! ==> The number should be either 1 or 2\r", 0);		
		}
	}
}

/**
  * @brief  Download a file via serial port to internal flash of MCU
  * @param  None
  * @retval None
  */
void extSerialDownload(uint32_t Addr) // Son : void --> uint32_t Addr
{
	uint8_t Number[10] = "          ";
	int32_t Size = 0;

	SerialPutString("Waiting for the file to be sent ... (press 'a' to abort)\n\r", 0);
	Size = ymodemReceive(&tab_1024[0], &extDownLoadDevice, Addr);
	if (Size > 0)
	{
		SerialPutString("\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ", 0);
		SerialPutString(file_name, 0);
		Int2Str(Number, Size);
		SerialPutString("\n\r Size: ", 0);
		SerialPutString(Number, 0);
		SerialPutString(" Bytes\r\n", 0);
		SerialPutString("-------------------\n\r", 0);
	}
	else if (Size == -1)
	{
		SerialPutString("\n\n\rThe image size is higher than the allowed space memory!\n\r", 0);
	}
	else if (Size == -2)
	{
		SerialPutString("\n\n\rVerification failed!\n\r", 0);
	}
	else if (Size == -3)
	{
		SerialPutString("\r\n\nAborted by user.\n\r", 0);
	}
	else
	{
		SerialPutString("\n\rFailed to receive the file!\n\r", 0);
	}
}

/**
  * @brief  Upload a file via serial port to internal flash of MCU
  * @param  None
  * @retval None
  */
void extSerialUpload(uint32_t Addr) //Son: void --> int32_t Addr
{
	uint8_t Number[10] = "          ";
	int32_t Size = 0;

	SerialPutString("Ready the file to be send to the PC ... \n\r", 0);
	Size = ymodemTransmit(&extUpLoadDevice, Addr );//Son: Add uint32_t Addr
	if (Size > 0)
	{
		SerialPutString("\n\n\r Upload database Completed Successfully!\n\r--------------------------------\r\n Name: ", 0);
		SerialPutString(file_name, 0);
		Int2Str(Number, Size);
		SerialPutString("\n\r Size: ", 0);
		SerialPutString(Number, 0);
		SerialPutString(" Bytes\r\n", 0);
		SerialPutString("-------------------\n\r", 0);
	}
	else if (Size == -1)
	{
		SerialPutString("\n\n\rThe image size is higher than the allowed space memory!\n\r", 0);
	}
	else if (Size == -2)
	{
		SerialPutString("\n\n\rVerification failed!\n\r", 0);
	}
	else if (Size == -3)
	{
		SerialPutString("\r\n\nAborted by user.\n\r", 0);
	}
	else
	{
		SerialPutString("\n\rFailed to receive the file!\n\r", 0);
	}
}

/*--------------------------------------------------*/
/* Opposite with extSerialUploadOpposite() Function              */
/*                                                                                 */
/*                                                                                 */
/*--------------------------------------------------*/
void extSerialUploadOpposite(uint32_t Addr)
{
	uint8_t Number[10] = "          ";
	int32_t Size = 0;

	SerialPutString("Ready the file to be send to the PC ... \n\r", 0);
	Size = ymodemTransmit(&exUpLoadDeviceOpposite, Addr );//Son: Add uint32_t Addr
	if (Size > 0)
	{
		SerialPutString("\n\n\r Upload database Completed Successfully!\n\r--------------------------------\r\n Name: ", 0);
		SerialPutString(file_name, 0);
		Int2Str(Number, Size);
		SerialPutString("\n\r Size: ", 0);
		SerialPutString(Number, 0);
		SerialPutString(" Bytes\r\n", 0);
		SerialPutString("-------------------\n\r", 0);
	}
	else if (Size == -1)
	{
		SerialPutString("\n\n\rThe image size is higher than the allowed space memory!\n\r", 0);
	}
	else if (Size == -2)
	{
		SerialPutString("\n\n\rVerification failed!\n\r", 0);
	}
	else if (Size == -3)
	{
		SerialPutString("\r\n\nAborted by user.\n\r", 0);
	}
	else
	{
		SerialPutString("\n\rFailed to receive the file!\n\r", 0);
	}
}

/* Append file */
/* @param:
	NumByteToWrite: number bytes of data (except the bytes of file size and file name )
	@return:
	1: write successfully
	-1 : not enough memory
	-2: Flash isnot ready
*/
int8_t AppendFile (uint8_t *Buf, uint32_t Addr, uint8_t *FileName, uint16_t NumByteToWrite)
{
	uint16_t FreeSpace,i; 
	int32_t Size;
	uint32_t WriteAddr;
	uint8_t FileSize[FILE_SIZE_LENGTH];
	if(SPI_FlashInf_IsReady())
	{
		if (CheckFileExist(FileName,Addr)==1)
		{
			SPI_FLASH_BufferRead(FileSize, Addr + FILE_NAME_LENGTH , FILE_SIZE_LENGTH);
			Str2Int(FileSize, &Size);
			FreeSpace = FILE_LOG_SIZE - (uint16_t)Size - FILE_NAME_LENGTH -FILE_SIZE_LENGTH -2;
			if (FreeSpace >= NumByteToWrite)
			{
				/* Clear sector buffer 16k */
				SPI_FlashInf_Erase(BUFFER_SECTOR);

				/* Copy data from first buffer of file to buffer sector */
				CopyData2Buf(Addr,Size);

				/* Clear first buffer of log file*/
				SPI_FlashInf_Erase(Addr);

				/* Copy data from buffer with out header have size of byte */
				CopyDataFromBuf(Addr,Size);

				/* --------------Write new data to log file --------------------------------*/
				/* start Address to write append*/
				WriteAddr = Addr + FILE_NAME_LENGTH + 1 + FILE_SIZE_LENGTH+ 1 + (uint16_t)Size;
				/* Write data */
				SPI_FLASH_BufferWrite( Buf,  WriteAddr, NumByteToWrite);
				/* ---------------------------------------------------------------------*/
				
				/* Write size of bytes */
				Size +=  NumByteToWrite;
				Int2Str(FileSize, Size);
				SPI_FLASH_BufferWrite(FileSize,  Addr + FILE_NAME_LENGTH , FILE_SIZE_LENGTH);
			}
			else
			{	
				if(FILE_LOG_SIZE >= NumByteToWrite + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 )
				{
					CreateLogFile(Buf, Addr, FileName, NumByteToWrite);
				}
				else
				{
					return -1;// not enough memory
				}
			}
		}
		else
		{				
			if(FILE_LOG_SIZE >= NumByteToWrite + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2)
			{
				CreateLogFile(Buf, Addr, FileName, NumByteToWrite);
			}
			else
			{
				return -1;// not enough memory
			}
		}
	}
	else
	{
		return -2;
	}
	
return 1;
}

/*--------------------------------------------*/
/* Read New Log File from address                           */
/*                                                                       */
/*                                                                       */
/*--------------------------------------------*/
void CreateLogFile(uint8_t* Buf, uint32_t Addr, uint8_t *FileName, uint16_t NumByteToWrite)
{
	uint16_t i;
	uint32_t WriteAddr;
	uint8_t FileSize[FILE_SIZE_LENGTH];

	/* Clear sector of this log file------ */
	for(i=0; i<(FILE_LOG_SIZE/LENGTH_SECTOR);i++)
	{
		SPI_FlashInf_Erase(Addr+ i*LENGTH_SECTOR);
	}
	
	/* Write file name, file size and datat */
	/* Write Data-------------------- */
	WriteAddr = Addr + FILE_NAME_LENGTH + 1 + FILE_SIZE_LENGTH + 1;
	SPI_FLASH_BufferWrite( Buf,  WriteAddr, NumByteToWrite);	

	/* Write FileName---------------- */
	SPI_FLASH_BufferWrite(FileName, Addr , FILE_NAME_LENGTH);
	
	/*Write FileSize------------------ */
	Int2Str(FileSize ,NumByteToWrite);
	SPI_FLASH_BufferWrite(FileSize,  Addr + FILE_NAME_LENGTH  , FILE_SIZE_LENGTH);
}

/*---------------------------------------------------*/ 
/*check whether Log File has existed in Flash memory             */
/*   return 1: existed                                                         */
/*   return 0: not existed                                                   */
/*---------------------------------------------------*/
uint8_t CheckFileExist(uint8_t * FileName, uint32_t Addr)
{
	uint8_t FileName_Buf[FILE_NAME_LENGTH +1];
	
      SPI_FLASH_BufferRead(FileName_Buf, Addr, FILE_NAME_LENGTH + 1);

	if(strcmp(FileName,FileName_Buf)==0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
		
}

/*--------------------------------------------*/
/* Copy Data From Address of sector to buffer sector  */
/*                                                                       */
/*                                                                       */
/*--------------------------------------------*/
void CopyData2Buf ( uint32_t Addr,uint32_t Size)
{
	uint8_t Buf_Temp[256];
	uint16_t i;
	if (Size + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2>= LENGTH_SECTOR)
	{
		for (i=0; i<256; i++)
		{
			SPI_FLASH_BufferRead(Buf_Temp ,Addr + i*256,256); 
			SPI_FLASH_BufferWrite(Buf_Temp,BUFFER_SECTOR + i*256,256);
		}
	}
	else
	{
		if ((Size + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2)%256)
		{
			for (i=0; i<((Size + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2)/256) + 1;i++)
			{
				SPI_FLASH_BufferRead(Buf_Temp ,Addr + i*256,256); 
				SPI_FLASH_BufferWrite(Buf_Temp,BUFFER_SECTOR + i*256,256);
			}
		}
		else
		{
			for (i=0; i<((Size + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2)/256);i++)
			{
				SPI_FLASH_BufferRead(Buf_Temp ,Addr + i*256,256); 
				SPI_FLASH_BufferWrite(Buf_Temp,BUFFER_SECTOR + i*256,256);
			}
		}
			
	}
}


/*=============================================*/
/* Copy Data From buffer sector to Address of sector            */
/*                                                                                 */
/*                                                                                 */
/*                                                                                  */
/*                                                                                 */
/*==============================================*/
void CopyDataFromBuf(uint32_t Addr, uint32_t Size)
{
	uint8_t Buf_Temp[256];
	uint16_t i;

	SPI_FLASH_BufferRead(Buf_Temp,BUFFER_SECTOR , FILE_NAME_LENGTH );
	SPI_FLASH_BufferWrite(Buf_Temp,Addr , FILE_NAME_LENGTH );

	SPI_FLASH_BufferRead(Buf_Temp,BUFFER_SECTOR + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2, 256 - FILE_NAME_LENGTH -FILE_SIZE_LENGTH -2);
	SPI_FLASH_BufferWrite(Buf_Temp,Addr + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 , 256 - FILE_NAME_LENGTH -FILE_SIZE_LENGTH -2 );

	if ((Size + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2)> LENGTH_SECTOR)
	{
		for (i=1; i<256; i++)
		{
			SPI_FLASH_BufferRead(Buf_Temp,BUFFER_SECTOR + i*256,256);
			SPI_FLASH_BufferWrite(Buf_Temp ,Addr + i*256,256); 
		}	
	}
	else
	{
		if((Size + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2) % 256)
		{
			for (i=1; i<(Size + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2)/256 +1; i++)
			{
				SPI_FLASH_BufferRead(Buf_Temp,BUFFER_SECTOR + i*256,256);
				SPI_FLASH_BufferWrite(Buf_Temp ,Addr + i*256,256); 
			}	
		}
		else
		{
			for (i=1; i<(Size + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2)/256 ; i++)
			{
				SPI_FLASH_BufferRead(Buf_Temp,BUFFER_SECTOR + i*256,256);
				SPI_FLASH_BufferWrite(Buf_Temp ,Addr + i*256,256); 
			}	
		}
	}
}


/*------------------------------------------------*/
/* Read data of Log File from First Address to Last Address */
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*------------------------------------------------*/
void ReadLogFile(uint8_t *Buf,uint8_t *FileName, uint32_t Addr,uint32_t First_Addr, uint32_t Last_Addr)
{
		 SPI_FLASH_BufferRead(Buf, Addr, (uint16_t)(Last_Addr - First_Addr));
}

/*------------------------------------------------*/
/* Write File with opposite address                                 */
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*                                                                             */
/*------------------------------------------------*/
int8_t AppendFileOpposite (uint8_t *Buf, uint32_t Addr, uint8_t *FileName, uint16_t NumByteToWrite)
{
	uint16_t FreeSpace,i; 
	int32_t Size;
	uint32_t WriteAddr;
	uint8_t FileSize[FILE_SIZE_LENGTH],FileSize_Opposite[FILE_SIZE_LENGTH],Buf_Op[256], FileSize_Op[FILE_SIZE_LENGTH];
	if(SPI_FlashInf_IsReady())
	{
		if (CheckFileExistOpposite(FileName,Addr)==1)
		{
			SPI_FLASH_BufferRead(FileSize_Opposite, Addr - FILE_NAME_LENGTH - FILE_SIZE_LENGTH + 1, FILE_SIZE_LENGTH);
			Opposite(FileSize,FileSize_Opposite,FILE_SIZE_LENGTH);	
			Str2Int(FileSize, &Size);
			FreeSpace = FILE_LOG_SIZE - (uint16_t)Size - FILE_NAME_LENGTH -FILE_SIZE_LENGTH -2;
			if (FreeSpace >= NumByteToWrite)
			{
				/* Clear sector buffer 16k */
				SPI_FlashInf_Erase(BUFFER_SECTOR);

				/* Copy data from first buffer of file to buffer sector */
				CopyData2Buf_Opposite(Addr,Size);
				
				/* Clear first buffer of log file*/
				SPI_FlashInf_Erase(Addr - LENGTH_SECTOR + 1);
				
				/* Copy data from buffer with out header have size of byte */
				CopyDataFromBuf_Opposite(Addr,Size);
				
				/* --------------Write new data to log file --------------------------------*/
				/* start Address to write append*/
				WriteAddr = Addr - FILE_NAME_LENGTH - 1 - FILE_SIZE_LENGTH- 1 - (uint16_t)Size;
				/* Write data */
				Opposite(Buf_Op,Buf,NumByteToWrite);
				SPI_FLASH_BufferWrite( Buf_Op,  WriteAddr - NumByteToWrite +1, NumByteToWrite);
				/* ---------------------------------------------------------------------*/
				
				/* Write size of bytes */
				Size +=  NumByteToWrite;
				Int2Str(FileSize, Size);
				Opposite(FileSize_Op,FileSize, FILE_SIZE_LENGTH);
				SPI_FLASH_BufferWrite(FileSize_Op,  Addr - FILE_NAME_LENGTH - FILE_SIZE_LENGTH +1, FILE_SIZE_LENGTH);
			}
			else
			{	
				if(FILE_LOG_SIZE >= NumByteToWrite + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 )
				{
					CreateLogFile_Opposite(Buf, Addr, FileName, NumByteToWrite);
				}
				else
				{
					return -1;// not enough memory
				}
			}
		}
		else
		{				
			if(FILE_LOG_SIZE >= NumByteToWrite + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2)
			{
				CreateLogFile_Opposite(Buf, Addr, FileName, NumByteToWrite);
			}
			else
			{
				return -1;// not enough memory
			}
		}
	}
	else
	{
		return -2;
	}
	
return 1;
}

/*******************************************************************************
* Function Name  : CheckFileExistOpposite
* Description    : Check if File existed
* Input          : FileName--> Name of File
*			   Addr --> Address of File
* Output         : None
* Return         : 1--> has existed
*                     0--> not existed
*******************************************************************************/
uint8_t CheckFileExistOpposite(uint8_t * FileName, uint32_t Addr)
{
	uint8_t FileName_Buf[FILE_NAME_LENGTH +1],i;
	uint8_t FileName_Buf_Opposite[FILE_NAME_LENGTH +1];
	
      SPI_FLASH_BufferRead(FileName_Buf_Opposite, Addr - (FILE_NAME_LENGTH + 1) +1, FILE_NAME_LENGTH + 1);
	for (i=0;i<FILE_NAME_LENGTH + 1; i++)
	{
		FileName_Buf[i] = FileName_Buf_Opposite[FILE_NAME_LENGTH + 1 - i -1];
	}
	if(strcmp(FileName,FileName_Buf)==0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
		
}

/*******************************************************************************
* Function Name  : Opposite
* Description    : resort data in buffer
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void Opposite(uint8_t *New,uint8_t *Old, uint16_t BUF_LENGTH)
{
	uint16_t i;
	for (i=0;i<BUF_LENGTH; i++)
	{
		New[i] = Old[BUF_LENGTH - i -1];
	}
}

/*******************************************************************************
* Function Name  : CopyData2Buf_Opposite
* Description    : Copydata from Buffer of file header to Sector Buffer  
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void CopyData2Buf_Opposite(uint32_t Addr,uint32_t Size)
{
	uint8_t Buf_Temp[256],Buf_Temp_Op[256];
	uint16_t i;
	Size += FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2;
	if (Size >= LENGTH_SECTOR)
	{
		for (i=0; i<256; i++)
		{
			SPI_FLASH_BufferRead(Buf_Temp_Op ,Addr - i*256 - 256 + 1,256);
			Opposite(Buf_Temp,Buf_Temp_Op,256);
			SPI_FLASH_BufferWrite(Buf_Temp,BUFFER_SECTOR + i*256,256);
		}
	}
	else
	{
		if (Size % 256)
		{
			for (i=0; i<Size/256 + 1; i++)
			{
				SPI_FLASH_BufferRead(Buf_Temp_Op ,Addr - i*256 - 256 + 1,256);
				Opposite(Buf_Temp,Buf_Temp_Op,256);
				SPI_FLASH_BufferWrite(Buf_Temp,BUFFER_SECTOR + i*256,256);
			}
			
		}
		else
		{
			for (i=0; i<Size/256; i++)
			{
				SPI_FLASH_BufferRead(Buf_Temp_Op ,Addr - i*256 - 256 + 1,256);
				Opposite(Buf_Temp,Buf_Temp_Op,256);
				SPI_FLASH_BufferWrite(Buf_Temp,BUFFER_SECTOR + i*256,256);
			}
		}
			
	}
}

/*******************************************************************************
* Function Name  : CopyDataFromBuf_Opposite
* Description    :    Copy data from Sector buffer to buffer of header without file size
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void CopyDataFromBuf_Opposite(uint32_t Addr,uint32_t Size)
{
	uint8_t Buf_Temp[256],Buf_Temp_Op[256];
	uint16_t i;

	
	SPI_FLASH_BufferRead(Buf_Temp_Op,BUFFER_SECTOR  , FILE_NAME_LENGTH );
	Opposite(Buf_Temp,Buf_Temp_Op,FILE_NAME_LENGTH);
	SPI_FLASH_BufferWrite(Buf_Temp,Addr - FILE_NAME_LENGTH + 1, FILE_NAME_LENGTH );

	
	SPI_FLASH_BufferRead(Buf_Temp_Op,BUFFER_SECTOR + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 , 256 - FILE_NAME_LENGTH -FILE_SIZE_LENGTH -2);
	Opposite(Buf_Temp,Buf_Temp_Op,256 - FILE_NAME_LENGTH -FILE_SIZE_LENGTH -2);
	SPI_FLASH_BufferWrite(Buf_Temp,Addr - FILE_NAME_LENGTH - FILE_SIZE_LENGTH - 2 -(256 - FILE_NAME_LENGTH -FILE_SIZE_LENGTH -2) + 1, 
		256 - FILE_NAME_LENGTH -FILE_SIZE_LENGTH -2 );
	
	Size += FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2;
	if (Size >= LENGTH_SECTOR)
	{
		for (i=1; i<256; i++)
		{
			
			SPI_FLASH_BufferRead(Buf_Temp_Op,BUFFER_SECTOR + i*256  ,256);
			Opposite(Buf_Temp,Buf_Temp_Op,256);
			SPI_FLASH_BufferWrite(Buf_Temp ,Addr - i*256 -256 + 1,256); 
		}	
	}
	else
	{
		if (Size%256)
		{
			for (i=1; i<Size/256 +1; i++)
			{
				
				SPI_FLASH_BufferRead(Buf_Temp_Op,BUFFER_SECTOR + i*256  ,256);
				Opposite(Buf_Temp,Buf_Temp_Op,256);
				SPI_FLASH_BufferWrite(Buf_Temp ,Addr - i*256 -256 + 1,256); 
			}	
		}
		else
		{
			for (i=1; i<Size/256 ; i++)
			{
				
				SPI_FLASH_BufferRead(Buf_Temp_Op,BUFFER_SECTOR + i*256  ,256);
				Opposite(Buf_Temp,Buf_Temp_Op,256);
				SPI_FLASH_BufferWrite(Buf_Temp ,Addr - i*256 -256 + 1,256); 
			}		
		}
	}
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void CreateLogFile_Opposite(uint8_t* Buf, uint32_t Addr, uint8_t *FileName, uint16_t NumByteToWrite)
{
	uint16_t i;
	uint32_t WriteAddr;
	uint8_t FileSize[FILE_SIZE_LENGTH],FileSize_Op[FILE_SIZE_LENGTH],FileName_Op[FILE_NAME_LENGTH],Buf_Op[256];

	/* Clear sector of this log file------ */
	for(i=0; i<(FILE_LOG_SIZE/LENGTH_SECTOR);i++)
	{
		
		SPI_FlashInf_Erase(Addr - i*LENGTH_SECTOR - LENGTH_SECTOR +1);
	}
	
	/* Write file name, file size and datat */
	/* Write Data-------------------- */
	WriteAddr = Addr - FILE_NAME_LENGTH - 1 - FILE_SIZE_LENGTH - 1;
	Opposite(Buf_Op,Buf,NumByteToWrite);
	SPI_FLASH_BufferWrite( Buf_Op,  WriteAddr -NumByteToWrite + 1 , NumByteToWrite);	

	/* Write FileName---------------- */
	Opposite(FileName_Op,FileName, FILE_NAME_LENGTH);
	SPI_FLASH_BufferWrite(FileName_Op, Addr - FILE_NAME_LENGTH +1, FILE_NAME_LENGTH);
	/*Write FileSize------------------ */
	Int2Str(FileSize ,NumByteToWrite);
	Opposite(FileSize_Op,FileSize,FILE_SIZE_LENGTH);
	SPI_FLASH_BufferWrite(FileSize_Op,  Addr - FILE_NAME_LENGTH - FILE_SIZE_LENGTH + 1 , FILE_SIZE_LENGTH);
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
uint8_t WriteFile_GPS_GSM(uint8_t *Data, int16_t NumByteToWrite, uint8_t SelectFile)
{
	/* Check free Space-----------------------------------*/
	if(CheckFreeSpace_GPS_GSM(NumByteToWrite,SelectFile))
	{
		/* Select file to write ------------------------------*/
		if (SelectFile == 1)
		{
			/* Write Data into GPS log file */
			AppendFileGPSOpposite(Data,NumByteToWrite);
		}
		else if (SelectFile == 2)
		{
			/* Write Data into GSM log file */
			AppendFileGSM(Data, NumByteToWrite);
		}
		else
		{
			return 2; // Transmit "SelectFile" argument incorrect
		}
	}
	else
	{
		return 0; //Not enough space to write
	}

return 1; // Write File success
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
uint8_t CheckFreeSpace_GPS_GSM(uint16_t Size, uint8_t SelectFile)
{
	uint32_t FileSize_GSM,FileSize_GPS,NumberSector_GMS,NumberSector_GPS,TotalSector;
	if (CheckFileExist("GSM.log", LAST_ADDR_MEM))
	{
		FileSize_GSM = ReadFileSize_GSM() + (SelectFile-1)*Size + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2;
	}
	else
	{
		FileSize_GSM = (SelectFile-1)*Size + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2;
	}

	if(CheckFileExistOpposite("GPS.log", ADDR_GSM_FILE))
	{
		FileSize_GPS = ReadFileSize_GPS() +  (2 - SelectFile )*Size +  FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2;
	}
	else
	{
		FileSize_GPS = (2 - SelectFile )*Size + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2;
	}
	
	if (FileSize_GSM%LENGTH_SECTOR)
	{
		NumberSector_GMS = FileSize_GSM/LENGTH_SECTOR + 1;
	}
	else
	{
		NumberSector_GMS = FileSize_GSM/LENGTH_SECTOR;
	}
	
	if (FileSize_GPS%LENGTH_SECTOR)
	{
		NumberSector_GPS = FileSize_GPS/LENGTH_SECTOR+1;
	}
	else
	{
		NumberSector_GPS = FileSize_GPS/LENGTH_SECTOR;
	}
	
	TotalSector = NumberSector_GMS + NumberSector_GMS;

	if( LENGTH_GPS_GSM_FILE/LENGTH_SECTOR >= TotalSector)
	{
		return 1;
	}
	else
	{
		return 0;	
	}
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
uint32_t ReadFileSize_GSM(void)
{
	uint32_t Size;
	uint8_t FileSize[FILE_SIZE_LENGTH];
	
	SPI_FLASH_BufferRead(FileSize, ADDR_GSM_FILE + FILE_NAME_LENGTH , FILE_SIZE_LENGTH);
	Str2Int(FileSize, &Size);
	
	return Size;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
uint32_t ReadFileSize_GPS(void)
{
	uint32_t Size;
	uint8_t FileSize[FILE_SIZE_LENGTH],FileSize_Opposite[FILE_SIZE_LENGTH];
	
	SPI_FLASH_BufferRead(FileSize_Opposite, LAST_ADDR_MEM - FILE_NAME_LENGTH - FILE_SIZE_LENGTH + 1, FILE_SIZE_LENGTH);
	Opposite(FileSize,FileSize_Opposite,FILE_SIZE_LENGTH);	
	Str2Int(FileSize, &Size);

	return Size;
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void FixData(uint8_t *FileName, uint32_t Addr_File, uint32_t Offset,uint8_t *NewData, uint16_t NumByteToFix)
{
	uint8_t NewData_Op[ EXT_FLASH_PAGE_SIZE ];
	uint16_t NumSector;
	uint32_t Addr_Insight,Addr_Sector,  Addr_Fix;

	/* Check type of File (write normally or invert ) ---------------------*/
	if (Addr_File == LAST_ADDR_MEM ) // File opposite
	{
             Addr_Fix = LAST_ADDR_MEM - Offset- (FILE_NAME_LENGTH + FILE_SIZE_LENGTH +2);
		/* Find sector will be fixed -----------------------------------*/
		NumSector = (Addr_File - Addr_Fix)/LENGTH_SECTOR;

		/* Erase sector --------------------------------------------*/
		SPI_FlashInf_Erase(BUFFER_SECTOR);
		
		/* Copy sector to buffer sector -------------------------------*/
		CopyData2Buf_Opposite( Addr_File - LENGTH_SECTOR*NumSector,LENGTH_SECTOR);
		
		/* Erase sector ---------------------------------------------*/
		SPI_FlashInf_Erase(Addr_File + 1 - LENGTH_SECTOR*(NumSector +1));		
		
		/* Copy data from buffer sector to the sector without data is fixed ---*/
		Addr_Insight = Addr_File - Addr_Fix - LENGTH_SECTOR*NumSector ;
		Addr_Sector = Addr_File                  - NumSector*LENGTH_SECTOR ;
		Copy2Buf_Opposite(   Addr_Insight, Addr_Sector,  NumByteToFix);

		/* Write data is fixed into the sector -----------------------------*/
		Opposite(NewData_Op,NewData,NumByteToFix);
		SPI_FLASH_BufferWrite( NewData_Op,  Addr_Fix - NumByteToFix +1, NumByteToFix);
		/* ----------------------------------------------------------*/
	}
	else // nomal file
	{
		Addr_Fix = Addr_File + Offset+ FILE_NAME_LENGTH + FILE_SIZE_LENGTH +2;
		
		/* Find sector will be fixed -------------------------------------*/
		NumSector = (Addr_Fix - Addr_File)/LENGTH_SECTOR;

		/* Erase sector ----------------------------------------------*/
		SPI_FlashInf_Erase(BUFFER_SECTOR);
		
		/* Copy sector to buffer sector ---------------------------------*/
		CopyData2Buf (Addr_File + LENGTH_SECTOR*NumSector,LENGTH_SECTOR);
		
		/* Erase sector -----------------------------------------------*/
		SPI_FlashInf_Erase(Addr_File + LENGTH_SECTOR*NumSector);
		
		/* Copy data from buffer sector to the sector without data is fixed ----*/
		Addr_Insight = Addr_Fix -(Addr_File + LENGTH_SECTOR*NumSector);
		Addr_Sector = Addr_File                  +  NumSector*LENGTH_SECTOR;
		Copy2Buf(   Addr_Insight,          Addr_Sector,         NumByteToFix);

		/* Write data is fixed into the sector -----------------------------*/
		SPI_FLASH_BufferWrite( NewData,  Addr_Fix , NumByteToFix);
	}

}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void Copy2Buf(uint32_t Addr_Insight,uint32_t Addr_Sector,uint16_t NumByteToFix)
{
	uint8_t Buf_Temp[256];
	uint16_t i,NoPage;

	NoPage = (Addr_Insight - BUFFER_SECTOR)/256;
	for (i=0; i<NoPage; i++)
	{
		SPI_FLASH_BufferRead(Buf_Temp,BUFFER_SECTOR  + i*256,256);
		SPI_FLASH_BufferWrite(Buf_Temp ,Addr_Sector    +  i*256,256); 
	}
	
	SPI_FLASH_BufferRead(Buf_Temp,BUFFER_SECTOR + NoPage*256  , Addr_Insight - NoPage*256);
	SPI_FLASH_BufferWrite(Buf_Temp,Addr_Sector    + NoPage*256  ,  Addr_Insight - NoPage*256);

	SPI_FLASH_BufferRead(Buf_Temp,BUFFER_SECTOR + Addr_Insight + NumByteToFix, 256 - Addr_Insight - NumByteToFix);
	SPI_FLASH_BufferWrite(Buf_Temp,Addr_Sector    + Addr_Insight + NumByteToFix,  256 - Addr_Insight - NumByteToFix);

	
	for (i=NoPage +1; i<256; i++)
	{
		SPI_FLASH_BufferRead(Buf_Temp,BUFFER_SECTOR + i*256,256);
		SPI_FLASH_BufferWrite(Buf_Temp,Addr_Sector     + i*256,256); 
	}	
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void Copy2Buf_Opposite(uint32_t Addr_Insight,uint32_t Addr_Sector,uint16_t NumByteToFix)
{
	uint8_t Buf_Temp[256], Buf_Temp_Op[256];
	uint16_t i,NoPage;

	NoPage = (Addr_Insight - BUFFER_SECTOR)/256;
	for (i=0; i<NoPage; i++)
	{
		SPI_FLASH_BufferRead(Buf_Temp_Op,BUFFER_SECTOR  + i*256,256);
		Opposite(Buf_Temp,Buf_Temp_Op,256);
		SPI_FLASH_BufferWrite(Buf_Temp ,Addr_Sector    -  i*256 -256 + 1,256); 
	}
	
	SPI_FLASH_BufferRead(Buf_Temp_Op,BUFFER_SECTOR + NoPage*256  , Addr_Insight - NoPage*256);
	Opposite(Buf_Temp,Buf_Temp_Op,  Addr_Insight - NoPage*256);
	SPI_FLASH_BufferWrite(Buf_Temp,Addr_Sector    - NoPage*256 -(Addr_Insight - NoPage*256) + 1 ,  Addr_Insight - NoPage*256);

	SPI_FLASH_BufferRead(Buf_Temp_Op,BUFFER_SECTOR + Addr_Insight + NumByteToFix, 256 - Addr_Insight - NumByteToFix);
	Opposite(Buf_Temp,Buf_Temp_Op, 256 - Addr_Insight - NumByteToFix);
	SPI_FLASH_BufferWrite(Buf_Temp,Addr_Sector    - Addr_Insight - NumByteToFix - (256 - Addr_Insight - NumByteToFix) +1,  256 - Addr_Insight - NumByteToFix);

	
	for (i=NoPage +1; i<256; i++)
	{
		SPI_FLASH_BufferRead(Buf_Temp_Op,BUFFER_SECTOR    + i*256,256);
		Opposite(Buf_Temp,Buf_Temp_Op,256);
		SPI_FLASH_BufferWrite(Buf_Temp,Addr_Sector - i*256 -256 +1,256); 
	}	
}



/*******************************************************************************
* Function Name  : 
* Description    :  Clear file  GSM in flash memory
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void ClearFile_GSM(void)
{
	uint32_t FileSize, NoSector,i, Addr;

	Addr = ADDR_GSM_FILE;
	/* Check number of sectors of GSM file */
	FileSize = ReadFileSize_GSM();

	if (FileSize % LENGTH_SECTOR)
	{
		NoSector = FileSize/LENGTH_SECTOR + 1;
	}
	else
	{
		NoSector = FileSize/LENGTH_SECTOR ;
	}
	
	/* Clear there sectors */
	for (i=0; i < NoSector; i++)
	{
		SPI_FlashInf_Erase(Addr + i*LENGTH_SECTOR);
	}

}

/*******************************************************************************
* Function Name  : 
* Description    :  Clear file  GPS in flash memory
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void ClearFile_GPS(void)
{
	uint32_t FileSize, NoSector,i, Addr;

	Addr = LAST_ADDR_MEM;
	
	/* Check number of sectors of GSM file */
	FileSize = ReadFileSize_GPS();

	if (FileSize % LENGTH_SECTOR)
	{
		NoSector = FileSize/LENGTH_SECTOR + 1;
	}
	else
	{
		NoSector = FileSize/LENGTH_SECTOR ;
	}
	
	/* Clear there sectors */
	for (i=0; i < NoSector; i++)
	{
		SPI_FlashInf_Erase(Addr + 1 - i*LENGTH_SECTOR - LENGTH_SECTOR );
	}
}

/*******************************************************************************
* Function Name  : 
* Description    :  different size with other file
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
int8_t AppendFileGPSOpposite (uint8_t *Buf, uint16_t NumByteToWrite)
{
	uint16_t i; 
	int32_t Size;
	uint32_t WriteAddr,Addr;
	uint8_t FileSize[FILE_SIZE_LENGTH],FileSize_Opposite[FILE_SIZE_LENGTH],Buf_Op[256], FileSize_Op[FILE_SIZE_LENGTH];
	uint8_t FileName[FILE_NAME_LENGTH];

	Addr = LAST_ADDR_MEM;
	sprintf (FileName,"GPS.log");
	if(SPI_FlashInf_IsReady())
	{
		if (CheckFileExistOpposite(FileName,Addr)==1)
		{
			SPI_FLASH_BufferRead(FileSize_Opposite, Addr - FILE_NAME_LENGTH - FILE_SIZE_LENGTH + 1, FILE_SIZE_LENGTH);
			Opposite(FileSize,FileSize_Opposite,FILE_SIZE_LENGTH);	
			Str2Int(FileSize, &Size);
				
			/* Clear sector buffer 16k */
			SPI_FlashInf_Erase(BUFFER_SECTOR);

			/* Copy data from first buffer of file to buffer sector */
			CopyData2Buf_Opposite(Addr,Size);
			
			/* Clear first buffer of log file*/
			SPI_FlashInf_Erase(Addr - LENGTH_SECTOR + 1);
			
			/* Copy data from buffer with out header have size of byte */
			CopyDataFromBuf_Opposite(Addr,Size);
			
			/* --------------Write new data to log file --------------------------------*/
			/* start Address to write append*/
			WriteAddr = Addr - FILE_NAME_LENGTH - 1 - FILE_SIZE_LENGTH- 1 - Size;
			/* Write data */
			Opposite(Buf_Op,Buf,NumByteToWrite);
			SPI_FLASH_BufferWrite( Buf_Op,  WriteAddr - NumByteToWrite +1, NumByteToWrite);
			/* ---------------------------------------------------------------------*/
			
			/* Write size of bytes */
			Size +=  NumByteToWrite;
			Int2Str(FileSize, Size);
			Opposite(FileSize_Op,FileSize, FILE_SIZE_LENGTH);
			SPI_FLASH_BufferWrite(FileSize_Op,  Addr - FILE_NAME_LENGTH - FILE_SIZE_LENGTH +1, FILE_SIZE_LENGTH);
			
			}
		else
		{				
				CreateLogFile_Opposite(Buf, Addr, FileName, NumByteToWrite);
		}
	}
	else
	{
		return -2;
	}
	
return 1;
}

/*******************************************************************************
* Function Name  : 
* Description    :  different size with other file
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
int8_t AppendFileGSM (uint8_t *Buf, uint16_t NumByteToWrite)
{
	uint16_t FreeSpace,i; 
	int32_t Size;
	uint32_t WriteAddr,Addr;
	uint8_t FileSize[FILE_SIZE_LENGTH],FileName[FILE_NAME_LENGTH];

	Addr = ADDR_GSM_FILE;
	sprintf (FileName,"GSM.log");
	if(SPI_FlashInf_IsReady())
	{
		if (CheckFileExist(FileName,Addr)==1)
		{
			SPI_FLASH_BufferRead(FileSize, Addr + FILE_NAME_LENGTH , FILE_SIZE_LENGTH);
			Str2Int(FileSize, &Size);
			
			/* Clear sector buffer 16k */
			SPI_FlashInf_Erase(BUFFER_SECTOR);

			/* Copy data from first buffer of file to buffer sector */
			CopyData2Buf(Addr,Size);

			/* Clear first buffer of log file*/
			SPI_FlashInf_Erase(Addr);

			/* Copy data from buffer with out header have size of byte */
			CopyDataFromBuf(Addr,Size);

			/* --------------Write new data to log file --------------------------------*/
			/* start Address to write append*/
			WriteAddr = Addr + FILE_NAME_LENGTH + 1 + FILE_SIZE_LENGTH+ 1 + (uint16_t)Size;
			/* Write data */
			SPI_FLASH_BufferWrite( Buf,  WriteAddr, NumByteToWrite);
			/* ---------------------------------------------------------------------*/
			
			/* Write size of bytes */
			Size +=  NumByteToWrite;
			Int2Str(FileSize, Size);
			SPI_FLASH_BufferWrite(FileSize,  Addr + FILE_NAME_LENGTH , FILE_SIZE_LENGTH);
			
		}
		else
		{				
				CreateLogFile(Buf, Addr, FileName, NumByteToWrite);
		}
	}
	else
	{
		return -2;
	}
	
return 1;
}
/**
  * @}
  */

/*******************(C)COPYRIGHT 2009 STMicroelectronics *****END OF FILE******/
