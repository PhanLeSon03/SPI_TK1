#include "EXTI_ACCE.h"
void EXTI_ButtonInit( void )
{    
      /*  Pin PC13 connect with button
	     Default connect with VDD
	     When external interrupt occur, the program jump into EXTI15_10_IRQHandler() function
	*/
        EXTI_InitTypeDef EXTI_InitStructure;	
	  
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);    
	   
	/* Eternal interrupt pin*/
       GPIO_InitTypeDef GPIO_InitStructure;  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource6);

#if 1
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);     
#endif

	/* Configure EXTI generate an interrupt */
	EXTI_ClearITPendingBit(EXTI_Line6);
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);     


//     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the EXTI Interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;// 3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}