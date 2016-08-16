
#include "stm32f10x.h"
#include "gsm.h"
#include "common.h"

/* Globalvariables ----------------------------------------------------------*/
u8 TxBuffer2[GSM_STBUFFSIZETX]; 
u8 RxBuffer2[GSM_STBUFFSIZERX];
u16 GSM_stLenSend;
__IO u16 TxCounter2 = 0x0000;
__IO u16 RxCounter2 = 0x0000;

u8 GSM_flgUARTTx;
u8 GSM_flgUARTRx;

//*****************************************************************************
//Description : Initialize GSM
//
//Parameter	: none
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GSM_Init(void)
{

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);

	/* Enable USART2 Clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
    /* NVIC configuration */
	NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	/* Configure the GPIO ports */
    GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GSM_ONOFF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GSM_CTS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GSM_RTS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    /* Configure USART2 Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = GSM_RX;
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
    /* Configure USART2 Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GSM_TX;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Enable the USART2 Pins Software Remapping */
    //GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

	USART_InitTypeDef USART_InitStructure;
	
	/* USART2 configured as follow:
		   - BaudRate = 9600 baud  
		   - Word Length = 8 Bits
		   - One Stop Bit
		   - No parity
		   - Hardware flow control disabled (RTS and CTS signals)
		   - Receive and transmit enabled
	 */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	
    /* Configure USART2 */
    USART_Init(USART2, &USART_InitStructure);

    /* Enable GSM */
	GPIO_SetBits(GPIOC, GSM_ONOFF);
	tmDelayms(250);
  	GPIO_ResetBits(GPIOC, GSM_ONOFF);
	tmDelayms(685);
	GPIO_SetBits(GPIOC, GSM_ONOFF);


    /* Enable USART2 Receive interrupts */     
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
    /* Enable USART2 */
 	USART_Cmd(USART2, ENABLE);
}

//void GSM_SendStr(const u8 * Str, u16 len)
//{
//
//    // len = 0 : print the string base on the terminated character - NULL
//	// len = 1 : print the string base on the lenght - don't care about NULL character
//	if(!len)	
//	{		
//		while (*(s + len) != '\0')
//		{	
//            while(GSM_flgUARTTx)
//            {
//                /* Write one byte to the transmit data register */
//                USART_SendData(USART2, (u16)*(Str + len++));
//            }
//		}
//	}
//    else
//    {
//        u16 n=0;
//        while ( n < len)
//        {
//		   /* send data to buffer */
//		   while(GSM_flgUARTTx)
//           {
//               /* Write one byte to the transmit data register */
//               USART_SendData(USART2, (u16)*(Str + n++));
//           }
//        }
//    }
//}

u8 *GSM_Recieve(void)
{
    /* The software must wait until TC=1. The TC flag remains cleared during all data
    transfers and it is set by hardware at the last frame’s end of transmission*/
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
    {}

    /* tight RTS to low to receive data*/
	GPIO_ResetBits(GPIOA,GSM_RTS);
	

    /* Enable the USART2 Receive interrupt: this interrupt is generated when the 
      USART2 receive data register is not empty */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	  
    while ((GSM_flgUARTRx == FALSE))
    {
         //SerialPutString(RxBuffer2,0);
    }

    GSM_flgUARTRx = FALSE;
	
    return RxBuffer2;
}
	

void GSM_Transmit(u8 *Data, u16 len)
{
    /* Copy data to buffer */
	if (len == 0) /* all sting*/
    {
       while (*(Data + len) != '\r')
	   {	
	       TxBuffer2[len] = *(Data + len);
		   len++;
	   }
	   GSM_stLenSend = len;
	   TxBuffer2[len] = Data[len];
    }
	else   /* string with length */
	{
        for (u8 i = 0; i< len; i++)
        {
            TxBuffer2[i] = *(Data + i);
        }
		GSM_stLenSend = len;
    
	}

	//Check the Clear to Send of GSM
    while (USART_GetFlagStatus(USART2,USART_FLAG_CTS)!=0);

     /* Enable the UART2 Transmoit interrupt: this interrupt is generated when the 
        UART2 transmit data register is empty */
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

	while(TxCounter2 < GSM_stLenSend)
	{
	}

	 /*Reset counter index */
	 TxCounter2 = 0;
     GSM_stLenSend = 0;
}
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        /* Read one byte from the receive data register */
        RxBuffer2[RxCounter2] = USART_ReceiveData(USART2);

        if((RxBuffer2[RxCounter2] == '\n'))
        {
            RxCounter2 = 0;
	        GSM_flgUARTRx = TRUE;

			/* Disable the USART2 Transmit interrupt */
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
        }
		else
		{
            RxCounter2++;
		}
    }
  
    if((USART_GetITStatus(USART2, USART_IT_TXE) != RESET) && (USART_GetFlagStatus(USART2,USART_FLAG_CTS)==0))
    {   
        /* Write one byte to the transmit data register */
        USART_SendData(USART2, TxBuffer2[TxCounter2]);

        /* Next character */
		TxCounter2++;
    

	    if ((TxCounter2 == GSM_stLenSend))
        {
            /* Write the last byte to the transmit data register */
            USART_SendData(USART2, TxBuffer2[TxCounter2]);

			/* Disable Interupt by USART2 buffer empty */	
	        USART_ITConfig(USART2, USART_IT_TXE, DISABLE); 
        }
    }
}


                                                                          \

 
