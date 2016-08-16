/**
  ******************************************************************************
  * @file    IAP/src/common.c 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    07/27/2009
  * @brief   This file provides all the common functions.
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
#include "common.h"
#include "usb_hw_config.h"
#include "usb_lib.h"
#include "usb_mem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  	Inserts a delay time with resolution is 0.2 milisecond..
  * @param  	nCount: specifies the delay time length.
  * @retval 	None
  */
void _delay(__IO uint32_t num)
{
	__IO uint32_t index = 0;

	/* default system clock is 72MHz */
	for(index = (7200 * num); index != 0; index--)
	{
	}
}

/**
  * @brief  Convert an Integer to a string
  * @param  str: The string
  * @param  intnum: The intger to be converted
  * @retval None
  */
void Int2Str(uint8_t* str, int32_t intnum)
{
	uint32_t i, Div = 1000000000, j = 0, Status = 0;

	for (i = 0; i < 10; i++)
	{
		str[j++] = (intnum / Div) + 0x30;

		intnum = intnum % Div;		
		
		Div /= 10;
		if ((str[j-1] == '0') & (Status == 0))
		{
			j = 0;
		}
		else
		{
			Status++;
		}
	}
	str[j++] = '\0';
}

/**
  * @brief  Convert a string to an integer
  * @param  inputstr: The string to be converted
  * @param  intnum: The intger value
  * @retval 1: Correct
  *         0: Error
  */
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
  uint32_t i = 0, res = 0;
  uint32_t val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
  {
    if (inputstr[2] == '\0')
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* over 8 digit hex --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0;i < 11;i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
      {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if (ISVALIDDEC(inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}

/**
  * @brief  Get an integer from the HyperTerminal
  * @param  num: The inetger
  * @retval 1: Correct
  *         0: Error
  */
uint32_t GetIntegerInput(int32_t * num)
{
	uint8_t inputstr[16];

	while (1)
	{
		GetInputString(inputstr);
		if (inputstr[0] == '\0') continue;
		if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0')
		{
			SerialPutString("User Cancelled \r\n", 0);
			return 0;
		}

		if (Str2Int(inputstr, num) == 0)
		{
			SerialPutString("Error, Input again: \r\n", 0);
		}
		else
		{
			return 1;
		}
	}
}

/**
  * @brief  Test to see if a key has been pressed on the HyperTerminal
  * @param  key: The key pressed
  * @retval 1: Correct
  *         0: Error
  */
uint32_t SerialKeyPressed(uint8_t *key)
{
#if 0
	if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
	{
		*key = (uint8_t)USART1->DR;
		return 1;
	}
	else
	{
		return 0;
	}
#else	 
	uint32_t cnt = usbInfRxCount();

	*key = usbInfRecieveByte();
	if(cnt)
	{
		return 1;
	}
	else
	{
		return 0;
	}
#endif
}

/**
  * @brief  Get a key from the HyperTerminal
  * @param  None
  * @retval The Key Pressed
  */
uint8_t GetKey(void)
{
	uint8_t key = 0;

	/* Waiting for user input */
	while (1)
	{
		_delay(5);
		if (SerialKeyPressed((uint8_t*)&key)) break;
	}
	return key;

}

/**
  * @brief  Print a character on the HyperTerminal
  * @param  c: The character to be printed
  * @retval None
  */
void SerialPutChar(uint8_t c)
{
	UserToPMABufferCopy(&c, ENDP1_TXADDR, 1);			
	SetEPTxCount(ENDP1, 1);
	SetEPTxValid(ENDP1);	
	_delay(20);				/*------------------------------*/
}

/**
  * @brief  Print a string on the HyperTerminal
  * @param  s: The string to be printed
  * @retval None
  */
void Serial_PutString(uint8_t *s, uint16_t len)
{	
	// len = 0 : print the string base on the terminated character - NULL
	// len = 1 : print the string base on the lenght - don't care about NULL character
	if(!len)	
	{		
		while (*(s + len) != '\0')
		{
			//SerialPutChar(*(s + len));		
			len++;
		}
	}
	
	// copy the transmited data to Packet Memory Area @phthevu 20100512
	{
		uint8_t n = 0, r = 0;

		n = len/BULK_MAX_PACKET_SIZE;
		r = len%BULK_MAX_PACKET_SIZE;
		for(uint8_t i = 0; i < n; i++)
		{
			UserToPMABufferCopy((uint8_t*)(s + i*BULK_MAX_PACKET_SIZE), ENDP1_TXADDR, BULK_MAX_PACKET_SIZE);			
			SetEPTxCount(ENDP1, BULK_MAX_PACKET_SIZE);
			SetEPTxValid(ENDP1);	
			//Insert delay to sure transmission is compltete, we should check transmission complete flag instead of delay
			_delay(1);
		}
		
		UserToPMABufferCopy((uint8_t*)(s + n*BULK_MAX_PACKET_SIZE), ENDP1_TXADDR, r);			
		SetEPTxCount(ENDP1, r);
		SetEPTxValid(ENDP1);	
		//Insert delay to sure transmission is compltete, we should check transmission complete flag instead of delay
		_delay(1);
	}
}

/**
  * @brief  Get Input string from the HyperTerminal
  * @param  buffP: The input string
  * @retval None
  */
void GetInputString (uint8_t * buffP)
{
	uint32_t bytes_read = 0;
	uint8_t c = 0;
	do
	{
		c = GetKey();
		if (c == '\r')
		{
			buffP[bytes_read] = '\r'; // 120213 : adding by sop1hc
			break;
		}
		if (c == '\b') /* Backspace */
		{
			if (bytes_read > 0)
			{
				SerialPutString("\b \b", 0);
				bytes_read --;
			}
			continue;
		}
		if (bytes_read >= CMD_STRING_SIZE )
		{
			SerialPutString("Command string size overflow\r\n", 0);
			bytes_read = 0;
			continue;
		}
		if (c >= 0x20 && c <= 0x7E)
		{
			buffP[bytes_read++] = c;
			//SerialPutChar(c);
		}
	}
	while (1);
	SerialPutString("\n\r", 0);
	buffP[bytes_read] = '\0';
}

/**
  * @}
  */

/*******************(C)COPYRIGHT 2009 STMicroelectronics *****END OF FILE******/
