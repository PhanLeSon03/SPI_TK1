/******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Version            : V3.0.1
* Date               : 04/27/2009
* Description        : Endpoint routines
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "usb_hw_config.h"
#include "usb_istr.h"
#include "globalconfig.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


uint8_t buffer_out[VIRTUAL_COM_PORT_DATA_SIZE];
__IO uint32_t count_out = 0;
__IO uint32_t count_in = 0;

/* re-ognize to ring buffer @nghoanganh 190410*/
u8 usb_buffer[USB_BUFFER_SIZE];
__IO uint32_t usb_count = 0;
__IO uint32_t usb_wr_index = 0;
__IO uint32_t usb_rd_index = 0;

/* This flag is set on usb Receiver buffer overflow  */
__IO  u8  usb_buffer_overflow;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP3_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
	uint32_t i;
	count_out = GetEPRxCount(ENDP3);
	PMAToUserBufferCopy(buffer_out, ENDP3_RXADDR, count_out);

	if(count_out >0)
	{

		/* add by nghoanganh 190410 to create ring buffer */
		for(i =0; i < count_out; i++)
		{
			usb_buffer[usb_wr_index]=buffer_out[i];

			if (++usb_wr_index == USB_BUFFER_SIZE) usb_wr_index=0;

			if (++usb_count == USB_BUFFER_SIZE)
			{
				usb_count=0;
				usb_buffer_overflow=1;
			}
		}
	}	
	SetEPRxValid(ENDP3);
}

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
	//count_in = 0;	
}

/*******************************************************************************
* Function Name  	: usbInfRxBufFlush
* Return         	: None
* Parameters 		: None
* Description		: Flush buffer for USB reception 
* Notes			: 
*******************************************************************************/
void usbInfRxBufFlush(void)
{
	usb_wr_index=0;
	usb_rd_index = 0;
	usb_count=0;
}

/*******************************************************************************
* Function Name  	: usbInfRxCount
* Return         	: None
* Parameters 		: None
* Description		: the number of valid data in rx buffer
* Notes			: 
*******************************************************************************/
uint32_t usbInfRxCount(void)
{
	uint32_t cnt = 0;
	
	//__disable_irq();
	cnt = usb_count;
	//__enable_irq(); 
	return cnt;
}

u8 usbInfRecieveByte(void)
{
	u8 data;

	//while(usb_count ==0);
	if(usb_count)
	{
		data=usb_buffer[usb_rd_index];
		if (++usb_rd_index == USB_BUFFER_SIZE) usb_rd_index=0;		
		
		// TODO: STM32 config 
		//__disable_irq();
		usb_count--;
		//__enable_irq(); 
		
		return data;
	}
	else
	{
		return '\0';
	}
}


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

