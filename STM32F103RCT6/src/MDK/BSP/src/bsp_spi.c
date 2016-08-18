/* Includes ------------------------------------------------------------------*/
#include "bsp_spi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t ByteSend2SPI, ByteReceive_SPI ,WaitSPI_Flag, Bsp_flgReceive;
__IO uint16_t RxIdx;
uint8_t SPI_SLAVE_Buffer_Rx[100];



/* Private function prototypes -----------------------------------------------*/
 void NVIC_Configuration(void);
/* Private functions ---------------------------------------------------------*/

void bspSPI_CS_Low(void)
{
	GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS);
}

void bspSPI_CS_High(void)
{
	GPIO_SetBits(GPIO_CS, GPIO_Pin_CS);
}

/*******************************************************************************
* Function Name  : bspSPI_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void bspSPI_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SPI2 and GPIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIO_CS |RCC_APB2Periph_GPIO_WP |RCC_APB2Periph_GPIO_HOLD |RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	 /* NVIC configuration ------------------------------------------------------*/
  	NVIC_Configuration();
	 
	/* Configure SPI2 pins: SCK, MISO and MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure I/O for Flash Chip select */
	////GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
	////GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	////GPIO_Init(GPIO_CS, &GPIO_InitStructure);

	/* Configure I/O for Flash Chip select */
	////GPIO_InitStructure.GPIO_Pin = GPIO_Pin_WP;
	////GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	////GPIO_Init(GPIO_WP, &GPIO_InitStructure);

	/* Configure I/O for Flash Chip select */
	/////GPIO_InitStructure.GPIO_Pin = GPIO_Pin_HOLD;
	/////GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	/////GPIO_Init(GPIO_HOLD, &GPIO_InitStructure);

	// enable write
	////GPIO_SetBits(GPIO_WP, GPIO_Pin_WP);
	////GPIO_SetBits(GPIO_HOLD, GPIO_Pin_HOLD);

	/* Deselect the FLASH: Chip Select high */
	////bspSPI_CS_High(); 	

	SPI_I2S_DeInit(SPI2);

	/* SPI2 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;//sop1hc SPI_Mode_Master
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	/* Enable SPI_MASTER TXE interrupt */
  	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);
  	/* Enable SPI_SLAVE RXNE interrupt */
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);

	/* Enable SPI2  */
	SPI_Cmd(SPI2, ENABLE);			
}


/*
spi1 clk PA5
spi1 MISO PA6
SPI1 MOSI PA7
*/
void bspSPI1_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SPI2 and GPIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	 /* NVIC configuration ------------------------------------------------------*/
  	NVIC_Configuration();
	 
	/* Configure SPI2 pins: SCK, MISO and MOSI */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);



	SPI_I2S_DeInit(SPI1);

	/* SPI2 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;//sop1hc SPI_Mode_Master
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	/* Enable SPI_MASTER TXE interrupt */
  	////SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);
  	/* Enable SPI_SLAVE RXNE interrupt */
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);

	/* Enable SPI2  */
	SPI_Cmd(SPI1, ENABLE);			
}


/*******************************************************************************
* Function Name  : SPI_FLASH_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
uint8_t bspSPI_ReadByte(void)
{
    
	return (bspSPI_SendByte(0xA5));
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
uint8_t bspSPI_SendByte(uint8_t byte)
{

	
#if 0
	uint8_t val = 0;
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TdeXE) == RESET);

	/* Send byte through the SPI2 peripheral */
	SPI_I2S_SendData(SPI2, byte);

	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

	/* get the data */
	val = SPI_I2S_ReceiveData(SPI2);

	/* Return the byte read from the SPI bus */
	return val;
#else
	ByteSend2SPI = byte;
	WaitSPI_Flag = 0;
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	/* Copy data to TX buffer                  */
	SPI_I2S_SendData(SPI2, ByteSend2SPI);
	/* Enable SPI_MASTER TXE interrupt */
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);
	/* Enable SPI_SLAVE RXNE interrupt */
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);

	/* Enable SPI2 */
    SPI_Cmd(SPI2, ENABLE);
	
	while (!WaitSPI_Flag);
    return  ByteReceive_SPI;
#endif
}


void SPI2_IRQHandler(void)
{
#if 0
	  if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_TXE) != RESET)
	  {

              //ByteReceive_SPI = SPI_I2S_ReceiveData(SPI2);
                  /* Send SPI_MASTER data */
              SPI_I2S_SendData(SPI2, 0x0F);
              //if ()
              //{
              //  /* Disable SPI_MASTER TXE interrupt */    
		      //  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
              //}
              
	   }
	  /* Store SPI_SLAVE received data */
	  if (SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) != RESET)
	  {
        /* Store SPI_SLAVE received data */
        if (RxIdx < 99)
            SPI_SLAVE_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI2);
        else
            ByteReceive_SPI = SPI_I2S_ReceiveData(SPI2);
			////ByteReceive_SPI = SPI_I2S_ReceiveData(SPI2);
		////SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
		WaitSPI_Flag = 1;
        Bsp_flgReceive = SET;
	  }
#endif
}

void SPI1_IRQHandler(void)
{
	  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_TXE) != RESET)
	  {

              //ByteReceive_SPI = SPI_I2S_ReceiveData(SPI2);
                  /* Send SPI_MASTER data */
              SPI_I2S_SendData(SPI1, SPI_SLAVE_Buffer_Rx[RxIdx]);
              //if ()
              //{
              //  /* Disable SPI_MASTER TXE interrupt */    
		      //SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
              //}
              
	   }
	  /* Store SPI_SLAVE received data */
	  if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) != RESET)
	  {
        /* Store SPI_SLAVE received data */
        if (RxIdx < 20)
        {
            SPI_SLAVE_Buffer_Rx[RxIdx++] = SPI_I2S_ReceiveData(SPI1);
        }
        else
        {
            RxIdx=0;
            SPI_SLAVE_Buffer_Rx[RxIdx] = SPI_I2S_ReceiveData(SPI1);
			////ByteReceive_SPI = SPI_I2S_ReceiveData(SPI2);
		////SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
		    WaitSPI_Flag = 1;
        }
        Bsp_flgReceive = SET;
	  }
}
/*

/*
return: True : finished receive
        False: TimeOut 
*/
bool SPI2_Receive(uint8_t * RxBuf, uint16_t Len , uint16_t TimeOut)
{
    uint32_t Time=0;
    /* Enable SPI_SLAVE RXNE interrupt */
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);
    SPI_Cmd(SPI2, ENABLE);
    RxIdx = 0;
    
    /* Transfer procedure */
    while (RxIdx < Len)
    {
        //SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
        Time++;
        //if (Time > TimeOut*1000)
        //{
        //    return 0;
        //}
    }

    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
    SPI_Cmd(SPI2, DISABLE);
    for (uint16_t i=0; i < Len; i++)
    {
        RxBuf[i] = SPI_SLAVE_Buffer_Rx[i]; 
    }
    

    return 1;
    

}

/*
return: True : finished receive
        False: TimeOut 
*/
bool SPI1_Receive(uint8_t * RxBuf, uint16_t Len , uint16_t TimeOut)
{
    uint32_t Time=0;
    /* Enable SPI_SLAVE RXNE interrupt */
    //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
    //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);
    //SPI_Cmd(SPI1, ENABLE);
    RxIdx = 0;
    
    /* Transfer procedure */
    while (RxIdx < Len)
    {
        //SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
        Time++;
        //if (Time > TimeOut*1000)
        //{
        //    return 0;
        //}
    }

    //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, DISABLE);
    //SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
    //SPI_Cmd(SPI1, DISABLE);
    for (uint16_t i=0; i < Len; i++)
    {
        RxBuf[i] = SPI_SLAVE_Buffer_Rx[i]; 
    }
    

    return 1;
    

}

/**********************************************************************
  * @brief  Configure the nested vectored interrupt controller.
  * @param  None
  * @retval None
  **********************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* 1 bit for pre-emption priority, 3 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Configure and enable SPI_MASTER interrupt -------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  
  /* Configure and enable SPI_MASTER interrupt -------------------------------*/
  NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}



//uint8_t bspSPI_SendByte(uint8_t byte)
//{

//      ByteSend2SPI = byte;
	/* Loop while DR register in not emplty */
//	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	/* Copy data to TX buffer                  */
//	SPI_I2S_SendData(SPI2, ByteSend2SPI);
	/* Enable SPI_MASTER TXE interrupt */
//      SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);
	 /* Enable SPI_SLAVE RXNE interrupt */
//      SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
	
//      return  1;
//}


