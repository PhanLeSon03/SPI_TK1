/**
  ******************************************************************************
  * @file 		Application\Main_Task\src\main.c 
  * @author  	Phan Le Son
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
#include "System_stm32f10x.h"
#include "Gps.h"
#include "stm32f10x.h"
#include "gsm.h"
#include "delayms.h"
#include "ADC.h"
#include "stm32f10x_conf.h"


#define DEBUG 0
      
__O u8 TimingDebounce;
static __O u32 TimingDelay;
static uint8_t stBttn, stBttnOld;
static uint8_t stPA0, stPA0Old;

extern uint8_t Request;
extern char GPSFix;
extern char GPSRun;
extern GPSPositionStruture GPSPosition;
extern __O float GPSSpeed;
extern u8 GPSTxBuffer[GPS_BUFFER_SIZE], GPSRxBuffer[GPS_BUFFER_SIZE];
extern uint8_t ByteSend2SPI, ByteReceive_SPI ,WaitSPI_Flag, Bsp_flgReceive;
extern __IO uint16_t RxIdx;
extern uint8_t SPI_SLAVE_Buffer_Rx[100];


BLOWFISH_CTX ctx;
extern void extFlashLoader(void);
void Delay1h(void);
void TimingDelay_Decrement(void);
void RCC_Configuration(void);
void NVIC_Configuration_GPS(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void TIMER2_Configuration(void);
void Move_FW(void);
void Move_BW(void);
void Stop(void);




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
uint8_t RxBuf[BUF_SIZE];
uint8_t RxBuf_Main[BUF_SIZE];
uint16_t cntRun;


int main( void )
{
	uint8_t buffer[100];
    
	uint32_t k,Index;
	int16_t vADCPA1;
    char ReceiveByte;
    char status;
	char	atr[] =  "AT+CREG?\r\n" ;
	u8 * buffer1;
    u8 Option,i;
	float Percent=0,Volt = 0;
	/* Setup the microcontroller hardware */
	SystemInit();

	/* interrupt config */
	InterruptConfig();

	// Initialize USB
	////usbInfInit();

	// Initialize serial flash
	///SPI_FlashInf_Init();
    
    
    //Initial I2C is used for Accelerometer
	////I2CInit();

	// Initial timer1
	tmInit();
	
	// Enable external interrupt by Accelerometer
	EXTI_ButtonInit();  
	 
	// Initial ADC2
	////senReadMONITOR_VBATInit();

	RCC_Configuration();
 	GPIO_Configuration();
	NVIC_Configuration_GPS();
	////USART_Configuration();
	//TIMER2_Configuration();
	////ADC_Configuration();

	// Initial GPS
	GPS_Init(); //USART 1 initialization

    bspSPI1_Init();

    // Initial GSM
    //GSM_Init();

	//tmDelayms(1000);
	
	//GPIO_SetBits(GPIOC,GPIO_Pin_8);              //PC8 : GSM_LED --> LED 2
	/*
	while(1)
	{

	}
	*/
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
		////Volt = senReadMONITOR_VBAT();
		////sprintf(buffer,"%0.2f",Volt);
		//SerialPutString(buffer, 0);
		//SerialPutString("\r\n", 0);
		////AppendFile("a", EXT_FLASH_FILE_ADDR4,"bat.txt", 1);
		////AppendFile(buffer, EXT_FLASH_FILE_ADDR4,"bat.txt", 4);
		////Delay1h();

        stBttn = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13); 
        stPA0 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);

		if ((stBttn==Bit_RESET)&&(stBttnOld==Bit_SET))
        {
        //    bspSPI_SendByte(0x0F);
        }
        if (1)
        {
		    ////for (int i=0;i <5;i++)
            ////{      
            ////    ReceiveByte = bspSPI_SendByte(atr[i]);
            ////    while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE))
            ////     {
            ////
            ////    }
            ////    USART_SendData(USART1, ReceiveByte);
            ////}
            //sprintf(buffer,"SPI Test\r\n");
            //for (uint16_t i =0; i < 10 ;i++)
            //{
            //    while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE))
            //    {

            //    }
            //    USART_SendData(USART1, buffer[i]); 
            //}
		    ///////GPIO_SetBits(GPIOC,GPIO_Pin_7);
		    ///////GPIO_SetBits(GPIOC,GPIO_Pin_12);
		    ///////tmDelayms(100);
            RxBuf_Main[0] = 0x00; 
            status = SPI1_Receive(RxBuf_Main, BUF_SIZE , 100);
 #if DEBUG          
            if (status==BUF_SIZE)
            {
                status = 0;
                
	            	
                sprintf(buffer,"\nSPI Data:");
                for (uint16_t i =0; i < 10 ;i++)
                {
                    while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE))
                    {

                    }
                    USART_SendData(USART1, buffer[i]); 
                }

                for (uint16_t i =0; i < BUF_SIZE  ;i++)
                {
                    while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE))
                    {

                    }
                    USART_SendData(USART1, RxBuf_Main[i]); 
                }
                
                while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE))
                {

                }
                USART_SendData(USART1, '\n');
                
	
            }
            else if (status!=0)
            {
                sprintf(buffer,"\nTime Out:");
                for (uint16_t i =0; i < 10 ;i++)
                {
                    while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE))
                    {

                    }
                    USART_SendData(USART1, buffer[i]); 
                }
                 for (uint16_t i =0; i < status  ;i++)
                {
                    while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE))
                    {

                    }
                    USART_SendData(USART1, RxBuf_Main[i]); 
                }

            }
            else
            {

            }   
#endif
            if (RxBuf_Main[0] =='i')
            {
                Move_FW();
                cntRun=0;
            }
            else if (RxBuf_Main[0] =='k')
            {
               Move_BW();
               cntRun =0;
            }
            else
            {
                cntRun++;
                if (cntRun >10)
                {
                    Stop();
                }
            }
                
		}
        else
        {	  

            //////GPIO_ResetBits(GPIOC,GPIO_Pin_7);            //PC7  : LED1 
            //////GPIO_ResetBits(GPIOC,GPIO_Pin_12);           //PC12: LED2
		    //SPI2 sending here    
		}
        
        stBttnOld = stBttn;
        stPA0Old = stPA0;
        //tmDelayms(10);
        //if (Bsp_flgReceive == SET)
        //{
        //    Bsp_flgReceive = RESET;
        //    USART_SendData(USART1, ByteReceive_SPI);
        //}
	}

	///while(1);
				///}
			///}
      	///}
#else
     char s[40];
     while(1)
	{	  					
		if(Virtual_Com_Port_GetDeviceState() == CONFIGURED)		// make sure Virtual COM Port is already setup
		{
			if(Request == SET_LINE_CODING)		// make sure User connected with Virtual COM Port for communication
			{	
				
				SerialPutString("Data of ADC: ",0);


				if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)!=0)
				{
				    GPIO_ResetBits(GPIOC,GPIO_Pin_7);            //PC7  : LED1 
				    GPIO_ResetBits(GPIOC,GPIO_Pin_12);           //PC12: LED2   
				    tmDelayms(1000);
				}
				else
				{
				    GPIO_SetBits(GPIOC,GPIO_Pin_7);
				    GPIO_SetBits(GPIOC,GPIO_Pin_12);
				    tmDelayms(1000);
				}

		        vADCPA1= GetADC1Channel(ADC_Channel_1);

				sprintf(buffer,"%d",(char *)vADCPA1);

				SerialPutString(buffer,0);
				SerialPutString("\r\n",0);
			}
		}
      	}
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
void RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_SPI1| RCC_APB2Periph_GPIOA|
                  RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 |RCC_APB1Periph_SPI2| RCC_APB1Periph_TIM2, ENABLE);
}

void NVIC_Configuration_GPS(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

 	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
}
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource10);
    
    //For Moutain Car
    // PA4 : Enable
    // PB0 : PWM
    // PA8 : Direction

    //PA4 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //PB0 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //PA8 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    
	EXTI_InitTypeDef EXTI_InitStructure;
//	EXTI_StructInit(& EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}


void USART_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART1, &USART_InitStructure);

	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

 	USART_Cmd(USART1, ENABLE);
 	USART_Cmd(USART2, ENABLE);
}

void TIMER2_Configuration()
{
	TIM_TimeBaseInitTypeDef Tim_TimeBaseStructure;

	Tim_TimeBaseStructure.TIM_Period = 1000;
	Tim_TimeBaseStructure.TIM_Prescaler = 36000-1;
	Tim_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	Tim_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &Tim_TimeBaseStructure);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM2, ENABLE);
}

void TimingDelay_Decrement(void)
{
	if(TimingDelay) TimingDelay--;
	if(TimingDebounce)
	{
		TimingDebounce--;
		if(TimingDebounce == 0x00)
		{
			EXTI_InitTypeDef EXTI_InitStructure;

			EXTI_InitStructure.EXTI_Line = EXTI_Line10;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);
		}
	}
}


void Move_FW(void)
{
    // go forward
    GPIO_SetBits(GPIOA,GPIO_Pin_4);
    GPIO_SetBits(GPIOB,GPIO_Pin_0);
    GPIO_SetBits(GPIOA,GPIO_Pin_8);
    ////tmDelayms(300);
    
    GPIO_ResetBits(GPIOC,GPIO_Pin_7);            //PC7  : LED1 
    GPIO_SetBits(GPIOC,GPIO_Pin_12);           //PC12: LED2


}

void Move_BW(void)
{
    // go backward
    GPIO_SetBits(GPIOA,GPIO_Pin_4);
    GPIO_SetBits(GPIOB,GPIO_Pin_0);
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);
    ////tmDelayms(300);
    
    GPIO_SetBits(GPIOC,GPIO_Pin_7);            //PC7  : LED1 
    GPIO_ResetBits(GPIOC,GPIO_Pin_12);           //PC12: LED2   

}

void Stop(void)
{
    GPIO_ResetBits(GPIOA,GPIO_Pin_4);
    GPIO_ResetBits(GPIOB,GPIO_Pin_0);
    GPIO_ResetBits(GPIOA,GPIO_Pin_8);
    GPIO_SetBits(GPIOC,GPIO_Pin_7);            //PC7  : LED1 
    GPIO_SetBits(GPIOC,GPIO_Pin_12);           //PC12: LED2


}


