#include "Delayms.h"
#include "stm32f10x_tim.h"
volatile uint32_t Mul_time;

void tmInit(void)
{

	uint16_t TimerPeriod;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	/* Compute the value to be set in ARR regiter to generate signal frequency at  */
	TimerPeriod =  72 - 1;

	/* TIM1 configuration */
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;          
	TIM_TimeBaseStructure.TIM_Prescaler = 999;       
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;

	/* Enable the TIM1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	/* Clear TIM1 update pending flag */
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);

}

void tmStart(void)
{
	/* Enable TIM1  Update interrupt */
	  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

	  /* TIM1 counter enable */
	  TIM_Cmd(TIM1, ENABLE);	
}

void tmStop(void)
{
	/*DISABLE TIM1 Update interrupt*/
	  TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);
	  
	  /* TIM1 counter DISABLE */
	  TIM_Cmd(TIM1, DISABLE);	 
}

void tmDelayms(uint32_t nCount)
{
          tmStart();
	    Mul_time = nCount;
          while (Mul_time);
  
	    tmStop();
}

void  TIM1_UP_IRQHandler(void)
{
	/* Clear TIM1 COM pending bit */
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	if (Mul_time > 0)
	{
      Mul_time--;
	} 
}

