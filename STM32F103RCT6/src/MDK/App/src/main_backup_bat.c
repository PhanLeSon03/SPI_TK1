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
void Delay1h(void);
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
        u8 Option,i;
	float Percent=0,Volt = 0;
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
	   
#if 1	
      ///while(1)
	///{	  					
		///if(Virtual_Com_Port_GetDeviceState() == CONFIGURED)		// make sure Virtual COM Port is already setup
		///{
			///if(Request == SET_LINE_CODING)		// make sure User connected with Virtual COM Port for communication
			///{	
	//Battery test
	//for (i = 0;i<48;i++ )
	//{
	while(1)
	{
		Volt = senReadMONITOR_VBAT();
		sprintf(buffer,"%0.2f",Volt);
		//SerialPutString(buffer, 0);
		//SerialPutString("\r\n", 0);
		AppendFile(", ", EXT_FLASH_FILE_ADDR4,"battery.txt", 2);
		AppendFile(buffer, EXT_FLASH_FILE_ADDR4,"battery.txt", 4);
		Delay1h();
	}

	///while(1);
				///}
			///}
      	///}
#else
	
#endif 
	
	/* Will only get here if there was not enough heap space to create the  idle task. */
	return 0;
}

void Delay1h(void)
{
	tmDelayms(1000*1800);
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



