/**
  ******************************************************************************
  * @file 		Application\Main_Task\src\main.c 
  * @author  	Phung The Vu
  * @version  	V1.0.0
  * @date  	22/04/2009
  * @brief  	Main program body.
  ******************************************************************************
  * @*Copyright 2009 TEKBOX DIGITAL SOLUTIONS Company. All Rights Reserved
  *
  */

/*
*************************************************************************************************************************************
*															   INCLUDE FILES													              *
*************************************************************************************************************************************
*/
#include "appconfig.h"
#include "Download.h"
#include "Langconfig.h"
#include "Blowfish.h"
#include "Battery.h"

extern uint8_t Request;
 BLOWFISH_CTX ctx;
extern void extFlashLoader(void);
/*******************************************************************************
* Function Name  	: InterruptConfig
* Return         	: None
* Parameters 		: None
* Created by		: 
* Date created	: 19/01/2009
* Description		: Configures the used IRQ Channels and sets their priority.
* Notes			: 
*******************************************************************************/
static void InterruptConfig(void);

//uint32_t AlternateLanguage= EXT_FLASH_FILE_ADDR;

 int main( void )
{
	uint32_t k,Index;
	int8_t buffer[200],buffer1[200];
        u8 Option;
	float Percent=0;
	/* Setup the microcontroller hardware */
	SystemInit();

	/* interrupt config */
	InterruptConfig();

	// Initialize USB
	usbInfInit();

	// Initialize serial flash
	SPI_FlashInf_Init();

       //Initial I2C is used for Accelerometer
	I2CInit();

	// Initial timer1
	tmInit();
	
	// Enable external interrupt by Accelerometer
	EXTI_ButtonInit();  
	 
	// Initial ADC2
	senReadMONITOR_VBATInit();

	GPIO_InitTypeDef GPIO_InitStructure;  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);	
	GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_SET);	



	// Blowfish Algorithm Initial
       //Blowfish_Init (&ctx, (unsigned char*)"TESTKEY", 7);
	   
	while(1)
	{	  					
		if(Virtual_Com_Port_GetDeviceState() == CONFIGURED)		// make sure Virtual COM Port is already setup
		{
			if(Request == SET_LINE_CODING)		// make sure User connected with Virtual COM Port for communication
			{																						
#if 0
				//SPI_FLASH_BulkErase();	
				//extFlashLoader();
				SerialPutString("\r\n================== Read File Log ============================\r\n\n", 0);
				SerialPutString("\r\n=========================================================\r\n\n", 0);
				//ClearFile_GPS();
				//ClearFile_GSM();
				//for(u16 i=0;i<60;i++)
				//	{
				//WriteFile_GPS_GSM("12345678123456\r\n", 16, 1);
				//	}
				//WriteFile_GPS_GSM("1234567890", 10, 2);/				WriteFile_GPS_GSM("2234567890", 10, 1);
				//WriteFile_GPS_GSM("2234567890", 10, 2);
				//WriteFile_GPS_GSM("3234567890", 10, 1);
				//WriteFile_GPS_GSM("3234567890", 10, 2);
				//WriteFile_GPS_GSM("3234567890", 10, 1);
				//FixData("GSM.log", ADDR_GSM_FILE, 20, "0000000000", 10);
				//FixData("GPS.log", LAST_ADDR_MEM, 20, "0000000000", 10);
				//AppendFile("123456789/n", EXT_FLASH_FILE_ADDR4,"a.log", 10);
				//AppendFile("223456789/n", EXT_FLASH_FILE_ADDR4,"a.log", 10);
				//AppendFile("323456789/n", EXT_FLASH_FILE_ADDR4,"a.log", 10);
				//AppendFile("423456789/n", EXT_FLASH_FILE_ADDR4,"a.log", 10);
				//for(k =0; k<64*1024/250 + 1; k++)
				//{
				//}
				
				//extSerialUploadOpposite(LAST_ADDR_MEM);
				//while(1);
				 //extSerialUpload(ADDR_GSM_FILE);
				extSerialUpload(EXT_FLASH_FILE_ADDR4);
				//GetSentence(buffer,EXT_FLASH_FILE_ADDR , Jan_idx);
                          //SerialPutString(buffer, 0);
				//SerialPutString("\r\n================== Fisnish================\r\n\n", 0);
				/*==================== BLOWFISH ALGORITHM ==========================*/
				//sprintf(buffer,"anh yeu em rat nhieu");
				//while(1)
				//{
				//GetInputString(buffer);
				//k = strlen(buffer);
				//Blowfish_Encrypt_String(&ctx,buffer, k);
				//SerialPutString("\r\n================== String after encrypt================\r\n\n", 0);
				//SerialPutString(buffer, 0);
				//Blowfish_Decrypt_String(&ctx,buffer, k);
				//SerialPutString("\r\n================== String after decrypt================\r\n\n", 0);
				//SerialPutString(buffer, 0);
				//SerialPutString("\r\n================== Type new string===================\r\n\n", 0);
				//}
#if 0			
		while(1)
		{
		GetInputString(buffer);
		switch (GetIndexCommand(buffer,&Option))
		{
		case Alarm_index:
			switch(Option)
			{
			case 1:
				Alarm_On();
				break;
			case 2:
				Alarm_Off();
				break;
			case 3:
				Alarm_Auto();
				break;
			default:
				SerialPutString("\r\nOption of command is not correct\r\n", 0);
				break;
			}
			break;

		case Language_index:
			switch (Option)
			{
			case 1:
				Language_OnOff(1,buffer1);
				SerialPutString(buffer1, 0);
				SerialPutString("\r\n", 0);
				break;
			case 2:
				Language_OnOff(2,buffer1);
				SerialPutString(buffer1, 0);
				SerialPutString("\r\n", 0);
				break;
			default:
				SerialPutString("\r\nOption of command is not correct\r\n", 0);
				break;
			}
			break;
		default:
			SerialPutString("\r\nCommand is not correct\r\n", 0);
			break;
		}

		
		//Language_OnOff(buffer,buffer1);
		//SerialPutString(buffer1, 0);
		//SerialPutString("\r\n", 0);
		//Alarm_On();
		//SerialPutString("\r\n", 0);
		}
#endif

#else				
				if(SPI_Check_Manufacturer_ID())
				{						
					// test read-write-erase with one sector
					Test_Flash_Page_to_Page(0);		
					// test read-write-erase with one sub-sector 8kb
					Test_Flash_Page_to_Page(1);	
					// test read-write-erase with one sub-sector 4kb
					Test_Flash_Page_to_Page(2);	
				}
#endif					
			}											
		}		
	}
	
	/* Will only get here if there was not enough heap space to create the  idle task. */
	return 0;
}


/*******************************************************************************
* Function Name  	: InterruptConfig
* Return         	: None
* Parameters 		: None
* Created by		: Phung The Vu
* Date created	: 19/01/2009
* Description		: Configures the used IRQ Channels and sets their priority.
* Notes			: 
*******************************************************************************/
static void InterruptConfig(void)
{ 
  	/* Set the Vector Table base address at 0x08000000 */
  	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);

  	/* Configure the Priority Group to 4 bits */
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);			/* 090128: NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); */
}

/*-----------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
		ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/**
  * @}
  */

/**
  * @}
  */ 




